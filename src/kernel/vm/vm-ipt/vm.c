/****************************************************************************
 *      $Id: vm.c,v 1.1 1998/03/19 23:40:56 kevine Exp $
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
#include <kernel/vm.h>
#include <kernel/kernel.h>
#include <r4kc0.h>
#include <alloca.h>
#include <l4/sigma0.h>
#include "ipt.h"

/****************************************************************************
 * vm_initial_as: 
 *
 * desc:- set up the initial page table with a page table entry for the
 * first task control block. Also set up anything required for future
 * sharing of kernel task control blocks.
 *
 * side effect:- sets kernel and task gpt pointer to point to the initial
 * page table.
 *
 ****************************************************************************/
void vm_mem_init(void)
{
  
  ipt_node_init();
  mt_mem_init();
}


ipt_t *tcb_ipt_insert(udw_t vaddr)
{
  ipt_t *ptr, *ipt;
  
  ipt = (ipt_t *) TLB2_BASE;
  
  /* form index */
  vaddr = vaddr >> 13;  
  ptr = &ipt[vaddr & TCB_ARRAY_SIZE_MASK];

  if (ptr->tag == -1ul)
  {
    /* null entry */
    ptr->tag = vaddr;
    ptr->pte0 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
      EL_Cache_CNWb |  EL_Valid | EL_Global;
    ptr->pte1 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
      EL_Cache_CNWb |  EL_Valid | EL_Global;
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
      ptr = (ipt_t *)ptr->link;
      if (ptr->tag == vaddr)
      {
	/* match this node */
	return ptr;
      }
    }
    
    /* ptr points to last link in chain */
    
    /* clash, so insert */
    ipt = ipt_node_alloc();
    ipt->tag = vaddr;
    ipt->pte0 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
      EL_Cache_CNWb |  EL_Valid | EL_Global;
    ipt->pte1 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
      EL_Cache_CNWb |  EL_Valid | EL_Global;
    ipt->link = ptr->link;
    ptr->link = (udw_t) ipt;
    return ipt;
  }
}

void vm_tcb_insert(as_t *a, dword_t v, dword_t paddr)
{
  ipt_t *l;
  
  l = tcb_ipt_insert(v);
  
  if (v & FRAMESIZE)
  {
    l->pte1 = paddr >>  FRAME_SIZE_SHIFT << 6 |
      EL_Cache_CNWb |  EL_Valid | EL_Dirty | EL_Global;
    
    
    if (!(l->pte0 &  EL_Global))
    {
      l->pte0 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
	EL_Cache_CNWb |  EL_Valid | EL_Global;
    }
  }
  else
  {
    l->pte0 = paddr >>  FRAME_SIZE_SHIFT << 6 |
      EL_Cache_CNWb |  EL_Valid | EL_Dirty | EL_Global;
    
    
    if (!(l->pte1 &  EL_Global))
    {
      l->pte1 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
	EL_Cache_CNWb |  EL_Valid | EL_Global;
    }
  }
  tlb_sync_shared(v);
}

void vm_tcb_unmap(as_t *as, dword_t base)
{
  ipt_t *ipt;
  udw_t vaddr, end, tag;
  ipt_t *ptr;
   
  ipt = (ipt_t *) TLB2_BASE;
  end =  base + (MAX_THREADS * TCB_SIZE) ;
  
  /* now wander down the tree to where we start doing real work */
  
  for (vaddr = base; vaddr < end; vaddr += 2*FRAMESIZE)
  {
    /* form index */
    tag = vaddr >> 13;  
    ptr = &ipt[tag & TCB_ARRAY_SIZE_MASK];
    
    if (ptr->tag == tag)
    {
      /* we have match at head */
      ptr->pte1 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
		EL_Cache_CNWb |  EL_Valid | EL_Global;
      ptr->pte0 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
		EL_Cache_CNWb |  EL_Valid | EL_Global;
    }
    else
    {
      /* a clash so we need to follow chain  */
      while (ptr->link != 0)
      {
	ipt = (ipt_t *) ptr->link;
	if (ipt->tag == tag)
	{
	  /* we have match at head */
	  ipt->pte1 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
	    EL_Cache_CNWb |  EL_Valid | EL_Global;
	  ipt->pte0 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
	    EL_Cache_CNWb |  EL_Valid | EL_Global;
	}
	ptr = ipt;
      }
    }
  }
}


void vm_sigz_insert_pte(dword_t vaddr, pte_t pte)
{
  ipt_t *l;
  kernel_vars *k;
  mt_node_t *tree_ptr;
  
  k = KERNEL_BASE;
  
  l = ipt_insert(k->sigz_tcb, vaddr);
  
  if (vaddr & FRAMESIZE)
  {
    l->pte1 = pte;
    tree_ptr = mt_mem_alloc();
    l->mt_ptr1 = (w_t) tree_ptr;
  }
  else
  {
    l->pte0 = pte;
    tree_ptr = mt_mem_alloc();
    l->mt_ptr0 = (w_t) tree_ptr;
    
  }
  tlb_sync(k->sigz_tcb,vaddr,pte);
     
#ifdef SIM
  {
    pte_t *tmp;
    tmp = vm_lookup_pte(k->sigz_tcb->gpt_pointer,vaddr);
    if (*tmp != pte) abort();
  }
#endif
  
  tree_ptr->parent = 0;
  tree_ptr->sister = 0;
  tree_ptr->child = 0;
  tree_ptr->vaddr = vaddr;
#ifndef SIM
  tree_ptr->task = (w_t) (SIGMA0_TID).ID;
#else
  tree_ptr->task = 0;
#endif  
}

void vm_check_map_trees(void *p)
{
}

extern char outbuff[];

void vm_print_fpage(as_t *a, dword_t v, dword_t s)
{
  unsigned int depth, depth_sum, leaf, entries, guard;
  ipt_t *ipt, *ptr;
  int i;
  ipt = (ipt_t *) a;

  depth_sum = entries = leaf = guard = 0;
  for (i = 0; i < IPT_ENTRIES; i++)
  {
    depth = 1;
    entries += 2;
    if (ipt[i].tag != -1ul)
    {
      if (ipt[i].pte0 & EL_Valid)
      {
	depth_sum += depth;
	leaf++;
      }
      if (ipt[i].pte1 & EL_Valid)
      {
	depth_sum += depth;
	leaf++;
      }
#if 0      
      printf("vaddr 0x%llx pte0 0x%x pte1 0x%x\n",
	     ipt[i].tag << 13, ipt[i].pte0,
	     ipt[i].pte1);
#endif      
      ptr = (ipt_t *) ipt[i].link;
      
      while (ptr != 0)
      {
	entries += 2;
	depth ++;
	if (ptr->pte0 & EL_Valid)
	{
	  depth_sum += depth;
	  leaf++;
	}
	if (ptr->pte1 & EL_Valid)
	{
	  depth_sum += depth;
	  leaf++;
	}
#if 0	
	printf("vaddr 0x%llx pte0 0x%x pte1 0x%x\n",
	       ptr->tag << 13, ptr->pte0,
	       ptr->pte1);
#endif	
	ptr = (ipt_t *)ptr->link;
      }
    }
  }
  
#ifdef SIM  
  ipt =  (ipt_t *) TLB2_BASE;

  for (i = 0; i < TCB_IPT_ENTRIES; i++)
  {
    if (ipt[i].tag != -1ul)
    {
      printf("vaddr 0x%llx pte0 0x%x pte1 0x%x\n",
	     ipt[i].tag << 13, ipt[i].pte0,
	     ipt[i].pte1);
      ptr = (ipt_t *) ipt[i].link;
      
      while (ptr != 0)
      {
	printf("vaddr 0x%llx pte0 0x%x pte1 0x%x\n",
	       ptr->tag << 13, ptr->pte0,
	       ptr->pte1);
	ptr = (ipt_t *)ptr->link;
      }
    }
  }
#endif

  
  sprintf(outbuff, "entries = %d leaf = %d  guard = %d\n\r",
	  entries, leaf, guard);
  printbuff();
  sprintf(outbuff, "depth sum = %d\n\r",
	  depth_sum);
  printbuff();
}



