/****************************************************************************
 *      $Id: ipt.c,v 1.2 1999/09/11 05:55:07 gernot Exp $
 *      Copyright (C) 1997, 1998 Kevin Elphinstone, University of New South
 *      Wales
 *
 *      This file is part of the L4/MIPS micro-kernel distribution.
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version 2
 *      of the License, or (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *      
 ****************************************************************************/
#include <r4kc0.h>
#include "ipt.h"
#include <kernel/panic.h>
#include <kernel/kmem.h>
#include <kernel/trace.h>
#include <kernel/vm.h>
#include <kernel/kernel.h>
#include <l4/types.h>
#include <l4/sigma0.h>
#include <alloca.h>
#ifdef SIM
#include <sim.h>
#include <stdio.h>
#endif

#define PAIRSIZE 13

cpt_t *cpt_insert(tcb_t *tcb, udw_t vaddr)
{
  /* assume interrupts are off */
  
  cpt_t *ptr, *cpt;
  int j;
  cpt = tcb->gpt_pointer;
  
  /* form index */
  vaddr = vaddr >> 15;

  
  ptr = (cpt_t *)((char *) cpt + (vaddr & CPT_ARRAY_SIZE_MASK) * 128);
  
  if (ptr->tag == -1ul)
  {
    /* null entry */
    ptr->tag = vaddr;
    for (j = 0; j < 8; j++)
    {
      ptr->pte[j] = 0;
    }
    ptr->link = 0;
      
    return ptr;
  }
  else if (ptr->tag == vaddr)
  {
    /* match in head */
    return ptr;
  }
  else
  {
    while (ptr->link != 0)
    {
      ptr = (cpt_t *)ptr->link;
      if (ptr->tag == vaddr)
      {
	/* match this node */
	return ptr;
      }
    }
    
    /* ptr points to last link in chain */
    
    /* clash, so insert */
    cpt = cpt_node_alloc();
#ifdef TLB_INSTR
    tcb->pt_size += CPT_OVERFLOW_NODE_SIZE;
#endif    
    cpt->tag = vaddr;
    for (j = 0; j < 8; j++)
    {
      cpt->pte[j] = 0;
    }
    cpt->link = ptr->link;
    ptr->link = (udw_t) cpt;
    return cpt;
  }
}

void cpt_remove(cpt_t *cpt, udw_t vaddr)
{
   /* assume interrupts are off */
  cpt_t *ptr;
  udw_t tag;
  
  /* form index */
  tag = vaddr >> 15;  
  ptr = (cpt_t *)((char *) cpt + (tag & CPT_ARRAY_SIZE_MASK) * 128);
  
  if (ptr->tag == -1ul)
  {
    /* no entry to remove */
    return;
  }
  else if (ptr->tag == tag)
  {
    ptr->pte[(vaddr >> 12) & 7] = 0;
  }
  else
  {
    /* a clash so we need to follow chain  */
    while (ptr->link != 0)
    {
      cpt = (cpt_t *) ptr->link;
      if (cpt->tag == tag)
      {
	/* we have match at head */
	cpt->pte[(vaddr >> 12) & 7] = 0;
      }
      ptr = cpt;
    }
  }
}




void vm_map(tcb_t *snd_tcb, dword_t snd_fpage, dword_t snd_cp,
                   tcb_t *rcv_tcb, dword_t rec_fpage)
{
  
  udw_t snd_base ,end, map_size;
  uw_t read_write_mask;
  dw_t align;

#ifdef KDEBUG
	  alphn_set("vmap");
#endif	  
   
  {
    udw_t snd_size_mask, rec_base;
    udw_t rec_size_mask;

    if ((snd_fpage & GPT_FPAGE_SIZE_MASK) == 256)
    {
      snd_fpage -= 4;	/* size=63, avoids overflow */
    }
    snd_size_mask = (1ul << ((snd_fpage & GPT_FPAGE_SIZE_MASK)
			     >> GPT_FPAGE_SIZE_SHIFT)) - 1;
    
    if ((rec_fpage & GPT_FPAGE_SIZE_MASK) == 256)
    {
      rec_size_mask = ~0ul;
    }
    else
    {
      rec_size_mask = (1ul << ((rec_fpage & GPT_FPAGE_SIZE_MASK)
			       >> GPT_FPAGE_SIZE_SHIFT)) - 1;
    }
    
    snd_base = snd_fpage & ~snd_size_mask |
      snd_cp & snd_size_mask & ~rec_size_mask;
    
    rec_base = rec_fpage & ~rec_size_mask |
      snd_cp & rec_size_mask & ~snd_size_mask;
    
    if (snd_base >= MAXUSEG || rec_base >= MAXUSEG) return;
    
    map_size = (snd_size_mask & rec_size_mask) + 1;
    
    if (snd_base + map_size >  MAXUSEG)
    {
      map_size = MAXUSEG - snd_base;
    }
    
    if (rec_base + map_size > MAXUSEG)
    {
      map_size = MAXUSEG - rec_base;
    }
    
    end = snd_base + map_size;
    align = rec_base - snd_base;
    
#ifdef _SIM
    printf("vm_map: sbase 0x%llx rbase 0x%llx msize 0x%llx\n",
	   snd_base, rec_base, map_size);
    fflush(stdout);
#endif
  }
  
  if (snd_fpage & L4_FPAGE_GRANT_MASK)
  {
    panic("L4 PANIC: Tried to grant fpage, not currently supported"); /* ok */
  }
  
  if (snd_fpage & L4_FPAGE_RW_MASK)
  {
    read_write_mask = ~ 0;
  }
  else
  {
    read_write_mask = ~  EL_Dirty;
  }
  if (map_size == 0)
  {
    return;
  }
  else if (map_size == FRAMESIZE)
  {
  /* single case */
    cpt_t *dst;
    pte_t *src_pte;
    udw_t dst_addr;

    /* do single page case */
    src_pte = vm_lookup_pte(snd_tcb->gpt_pointer, snd_base);
    if (src_pte != 0 && (*src_pte & EL_Valid))
    {
      int index;
      dst_addr = snd_base + align;
      index = (dst_addr >> 12) & 7;
      dst = cpt_insert(rcv_tcb, dst_addr);
      cpt_map(rcv_tcb, dst_addr, &dst->mt_ptr[index],  &dst->pte[index],
	      *src_pte, *(w_t *) ((char *)src_pte + 40), read_write_mask);
    }
  }
  else if (map_size <= 8*FRAMESIZE)
  {
    cpt_t *src, *dst;
    src = cpt_lookup_bucket(snd_tcb->gpt_pointer, snd_base);
    if (src != 0)
    {
      udw_t dst_addr;
      int j;
      dst_addr = snd_base+align;
      dst = cpt_insert(rcv_tcb, dst_addr);
      
      for (j = (snd_base >> 12) & 7;
	   j <= (((end -1)>> 12) & 7);
	   j++, dst_addr += FRAMESIZE)
      {
	int i;
	i = (dst_addr >> 12) & 7;
	cpt_map(rcv_tcb, dst_addr,
		&dst->mt_ptr[i],&dst->pte[i],
		src->pte[j], src->mt_ptr[j], read_write_mask);
      }
    }
  }
  else if (map_size < CPT_COVERAGE)
  {
    udw_t vaddr;
    cpt_t *src, *dst, *src_cpt, *dst_cpt;

    src_cpt = snd_tcb->gpt_pointer;
    dst_cpt = rcv_tcb->gpt_pointer;

    if (src_cpt == dst_cpt)
    {
      panic("L4 PANIC: don't support mapping from/to same page table");
    }
    
    for (vaddr = snd_base & ~(udw_t)(8* FRAMESIZE - 1);
	 vaddr < end;
	 vaddr += 8* FRAMESIZE)
    {
      
      src = cpt_lookup_bucket(src_cpt, vaddr);
      if (src != 0)
      {
	udw_t dst_addr;
	int j;
	dst_addr = vaddr+align;
	
	/* get leaf in dest */
	dst = cpt_insert(rcv_tcb, dst_addr);
#ifdef KDEBUG
	alphn_set("vmp2");
#endif	  
	for (j = 0;
	     j < 8;
	     j++, dst_addr += FRAMESIZE)
	{
	  cpt_map(rcv_tcb, dst_addr,
		  &dst->mt_ptr[j],&dst->pte[j],
		  src->pte[j], src->mt_ptr[j], read_write_mask);
	}   /* get pte */
      }
    }
  }
  else
  {
    /* loop through each entry in CPT */
    w_t i;
    cpt_t *cpt, *dst;
    udw_t dst_addr, vaddr;
    
    cpt = snd_tcb->gpt_pointer;
    for (i = 0; i < CPT_ENTRIES; i++)
    {
      /* for each entry in head */
      cpt_t *ptr;
      ptr = (cpt_t *)((char *) cpt + (i * 128));
      vaddr = ptr->tag << 15;
		      
      if (snd_base <= vaddr && vaddr < end)
      {
	int j;
	dst_addr = vaddr+align;
	dst = cpt_insert(rcv_tcb, dst_addr);
	for (j = 0;
	     j < 8;
	     j++, dst_addr += FRAMESIZE)
	{
	  cpt_map(rcv_tcb, dst_addr,
		  &dst->mt_ptr[j],&dst->pte[j],
		  ptr->pte[j], ptr->mt_ptr[j], read_write_mask);
	}
      }
      
      while (ptr->link != 0)
      {
	ptr = (cpt_t *) ptr->link;
	vaddr = ptr->tag << 15;
	
	if (snd_base <= vaddr && vaddr < end)
	{
	  int j;
	  dst_addr = vaddr+align;
	  dst = cpt_insert(rcv_tcb, dst_addr);
	  for (j = 0;
	       j < 8;
	       j++, dst_addr += FRAMESIZE)
	  {
	    cpt_map(rcv_tcb, dst_addr,
		    &dst->mt_ptr[j],&dst->pte[j],
		    ptr->pte[j], ptr->mt_ptr[j], read_write_mask);
	  }
	}
      }
    }
  }
}

  
void vm_fpage_unmap(tcb_t *tcb, dword_t fpage, dword_t mask)
{
  udw_t base, vaddr, end;
  uw_t read_write_mask;
  ub_t s1;
  
  
  s1 =  ((fpage & GPT_FPAGE_SIZE_MASK) >> GPT_FPAGE_SIZE_SHIFT);
  
  if (s1 < 12) s1 = 12;
  if (s1 > 63) s1 = 63;
  
  base = fpage & ~((1ul << s1) -1);
  
  if (base >=  MAXUSEG) return;
  
  end =  base + (1ul << s1) ;
  if (end > MAXUSEG) end = MAXUSEG;

  if (s1 == FRAME_SIZE_SHIFT)
  {
    pte_t *pte;
    
    pte = vm_lookup_pte(tcb->gpt_pointer,base);
    if (pte != 0)
    {
      cpt_unmap(pte,
		(mt_node_t *)(*(w_t *) ((char *)pte + 40)),
		read_write_mask,
		tcb, mask);
    }
  }
  else if ((1ul << s1) <= 8*FRAMESIZE)
  {
    cpt_t *src;
    src = cpt_lookup_bucket(tcb->gpt_pointer, base);
    if (src != 0)
    {
      int j;
      for (j = (base >> 12) & 7;
	   j <= (((end-1) >> 12) & 7);
	   j++)
      {
	cpt_unmap(&src->pte[j],
		  (mt_node_t *)src->mt_ptr[j],
		  read_write_mask,
		  tcb, mask);
      }
    }
  }
  else if ((1ul << s1) < CPT_COVERAGE)
  {
    for (vaddr = base;
	 vaddr < end;
	 vaddr += 8*FRAMESIZE)
    {
      cpt_t *ptr;
      /* do unmaping */
      ptr = cpt_lookup_bucket(tcb->gpt_pointer, vaddr);
      if (ptr != 0)
      {
	int j;
	for (j = 0;
	     j < 8;
	     j++)
	{
	  cpt_unmap(&ptr->pte[j], (mt_node_t *)ptr->mt_ptr[j], read_write_mask,
		    tcb, mask);
	}
      }
    }
  }
  else
  {
    w_t i,j;
    cpt_t *cpt;
    cpt = tcb->gpt_pointer;

    for (i = 0; i < CPT_ENTRIES; i++)
    {
      /* for each entry in head */
      cpt_t *ptr;
      ptr = (cpt_t *)((char *) cpt + (i * 128));
      vaddr = ptr->tag << 15;

      if (base <= vaddr && vaddr < end)
      {
	for (j = 0;
	     j < 8;
	     j++)
	{
	  cpt_unmap(&ptr->pte[j], (mt_node_t *)ptr->mt_ptr[j], read_write_mask,
		    tcb, mask);
	}
      }

      while (ptr->link != 0)
      {
	ptr = (cpt_t *) ptr->link;
	vaddr = ptr->tag << 15;

	if (base <= vaddr && vaddr < end)
	{
	  for (j = 0;
	       j < 8;
	       j++)
	  {
	    cpt_unmap(&ptr->pte[j], (mt_node_t *)ptr->mt_ptr[j],
		      read_write_mask,
		      tcb, mask);
	  }
	}
      }
    }
  }
}


void tcb_ipt_init(void)
{
  ipt_t *ipt;
  udw_t i;
  ipt = (ipt_t *)TLB2_BASE;

  for (i = 0; i < TCB_IPT_ENTRIES; i++)
  {
    ipt[i].tag = -1ul;
    ipt[i].link = 0;
  }
}

cpt_t *cpt_init(void)
{
  
  cpt_t *cpt, *ptr;
  udw_t i;
  cpt = cpt_head_alloc();
  
  for (i = 0; i < CPT_ENTRIES; i++)
  {
    ptr = (cpt_t *)((char *) cpt + (i * 128));
    ptr->tag = -1ul;
    ptr->link = 0;
  }
  return cpt;
}


  
void cpt_free(cpt_t *cpt)
{
  cpt_t *ptr;
  udw_t i;
  
  for (i = 0; i < CPT_ENTRIES; i++)
  {
    ptr = (cpt_t *)(((cpt_t *)((char *) cpt + (i * 128)))->link);
    
    while (ptr != 0)
    {
      cpt_node_free(ptr);
      ptr = (cpt_t *)ptr->link;
    }
  }
  cpt_head_free(cpt);
}



void cpt_map(tcb_t *rcv_tcb, udw_t dst_addr, w_t *mt_ptr, uw_t *pte,
	     uw_t src_pte, w_t src_mt_ptr, uw_t read_write_mask)
{
  if (src_pte & EL_Valid)
  {
    if (!(*pte & EL_Valid))
    {
      mt_node_t *tree_ptr;
      tree_ptr = (mt_node_t *) mt_mem_alloc();
      
      *mt_ptr = (w_t)tree_ptr;
      *pte = src_pte & read_write_mask;
#ifdef TLB_INSTR
      rcv_tcb->pt_number++;
#endif		  
      tlb_sync(rcv_tcb, dst_addr, *pte);
      
      tree_ptr->parent = src_mt_ptr;
      tree_ptr->sister = ((mt_node_t *)tree_ptr->parent)->child;
      tree_ptr->child = 0;
      tree_ptr->vaddr = dst_addr;
      tree_ptr->task = rcv_tcb->myself;
      ((mt_node_t *)tree_ptr->parent)->child = (w_t) tree_ptr;
    }
    else
    {
      if ((*pte &  EL_PFN) == (src_pte & EL_PFN))
      {
	*pte = src_pte & read_write_mask;
	tlb_sync(rcv_tcb, dst_addr, *pte);
      }
      /* else we already have a mapping, leave as is */
    }
  }
}
