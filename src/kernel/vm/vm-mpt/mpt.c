/****************************************************************************
 *      $Id: mpt.c,v 1.4 1999/09/11 05:55:08 gernot Exp $
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
#include "mpt.h"

#include <r4kc0.h>
#include <kernel/kmem.h>
#include <kernel/panic.h>
#ifdef SIM
#include <stdio.h>
#endif

mpt_t *mpt_init(void)
{
  mpt_t *mpt;
  udw_t *iptr;
  w_t i;
  /* TRACE(T_PROCS); */
    

  if ( mpt = k_frame_alloc() )
  {
    iptr = (udw_t *) mpt;
    for (i = 0; i < 4; i++)
    {
      iptr[i] = 0;
    }
  }
  return (mpt_t *)((udw_t)mpt | 3);
}

mpt_leaf_t *mpt_insert(tcb_t *tcb, udw_t vaddr)
{
  mpt_t *ptr, val;
  mpt_leaf_t *leaf_ptr;
  udw_t *iptr;
  int i ,j;
  
/****************************************************************************
 * ugly hack -- if called with bit 0 set, it's a gpt pointer, else it's
 * a tcb pointer 
 ****************************************************************************/

  
  if ((udw_t)tcb & 1)  
  {
    ptr = (mpt_t *) ((udw_t)tcb - 3);
  }
  else
  {
    ptr = (mpt_t *)((dword_t) tcb->gpt_pointer - 3);  /* 'aligned' pointer */
  }
  
  i = vaddr >> 61;
  ptr = &ptr[i];
  if (*ptr == 0)
  {
    *ptr = (w_t)  k_frame_alloc();
#ifdef TLB_INSTR
    if(tcb)
    {
      tcb->pt_size += 4096;
    }
    else
    {
      panic("L4 PANIC: TLB INSTR null tcb pointer");
    }
#endif        
    if (*ptr == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, iptr = (udw_t *) *ptr;
	 j < 512; j++)
    {
      iptr[j] = 0;
    }
  }
  
  i = (vaddr >> 51) & 01777;
  ptr = &((mpt_t *) *ptr)[i];
  if (*ptr == 0)
  {
    *ptr = (w_t)  k_frame_alloc();
#ifdef TLB_INSTR
    if(tcb)
    {
      tcb->pt_size += 4096;
    }
    else
    {
      panic("L4 PANIC: TLB INSTR null tcb pointer");
    }
#endif        
    if (*ptr == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, iptr = (udw_t *) *ptr;
	 j < 512; j++)
    {
      iptr[j] = 0;
    }
  }
  
  i = (vaddr >> 41) & 01777;
  ptr = &((mpt_t *) *ptr)[i];
  if (*ptr == 0)
  {
    *ptr = (w_t)  k_frame_alloc();
#ifdef TLB_INSTR
    if(tcb)
    {
      tcb->pt_size += 4096;
    }
    else
    {
      panic("L4 PANIC: TLB INSTR null tcb pointer");
    }
#endif        
    if (*ptr == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, iptr = (udw_t *) *ptr;
	 j < 512; j++)
    {
      iptr[j] = 0;
    }
  }
  
  i = (vaddr >> 31) & 01777;
  ptr = &((mpt_t *) *ptr)[i];
  if (*ptr == 0)
  {
    *ptr = (w_t)  k_frame_alloc();
#ifdef TLB_INSTR
    if(tcb)
    {
      tcb->pt_size += 4096;
    }
    else
    {
      panic("L4 PANIC: TLB INSTR null tcb pointer");
    }
#endif        
    if (*ptr == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, iptr = (udw_t *) *ptr;
	 j < 512; j++)
    {
      iptr[j] = 0;
    }
  }
  
  i = (vaddr >> 21) & 01777;
  ptr = &((mpt_t *) *ptr)[i];
  if (*ptr == 0)
  {
    *ptr = (w_t)  k_frame_alloc();
#ifdef TLB_INSTR
    if(tcb)
    {
      tcb->pt_size += 4096;
    }
    else
    {
      panic("L4 PANIC: TLB INSTR null tcb pointer");
    }
#endif        
    if (*ptr == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, iptr = (udw_t *) *ptr;
	 j < 512; j++)
    {
      iptr[j] = 0;
    }
  }
  
  
  leaf_ptr = (mpt_leaf_t *) *ptr;
  i = (vaddr >> 13) & 0377;
  return &leaf_ptr[i];
}

mpt_leaf_t *tcb_mpt_insert(tcb_t *tcb, udw_t vaddr)
{
  mpt_t *ptr, val;
  udw_t *new_ptr;
  mpt_leaf_t *leaf_ptr;
  int i ,j;
  
/****************************************************************************
 * ugly hack -- if called with bit 0 set, it's a gpt pointer, else it's
 * a tcb pointer 
 ****************************************************************************/

  
  if ((udw_t)tcb & 1)  
  {
    ptr = (mpt_t *) ((udw_t)tcb - 3);
  }
  else
  {
    ptr = (mpt_t *)((dword_t) tcb->gpt_pointer - 3);    /* 'aligned' pointer */
  }
  
  i = vaddr >> 61;
  if (ptr[i] == 0)
  {
    ptr[i] = (w_t)  k_frame_alloc();
    if (ptr[i] == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, new_ptr = (udw_t *) ptr[i];
	 j < 512; j++)
    {
      new_ptr[j] = 0;
    }
  }
  
  ptr = (mpt_t *) ptr[i];
  i = (vaddr >> 51) & 01777;
  if (ptr[i] == 0)
  {
    ptr[i] = (w_t)  k_frame_alloc();
    if (ptr[i] == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, new_ptr = (udw_t *) ptr[i];
	 j < 512; j++)
    {
      new_ptr[j] = 0;
    }
  }
  
  ptr = (mpt_t *) ptr[i];
  i = (vaddr >> 41) & 01777;
  if (ptr[i] == 0)
  {
    ptr[i] = (w_t)  k_frame_alloc();
    if (ptr[i] == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, new_ptr = (udw_t *) ptr[i];
	 j < 512; j++)
    {
      new_ptr[j] = 0;
    }
  }
  
  ptr = (mpt_t *) ptr[i];
  i = (vaddr >> 31) & 01777;
  if (ptr[i] == 0)
  {
    ptr[i] = (w_t)  k_frame_alloc();
    if (ptr[i] == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, new_ptr = (udw_t *) ptr[i];
	 j < 512; j++)
    {
      new_ptr[j] = 0;
    }
  }
  
  ptr = (mpt_t *) ptr[i];
  i = (vaddr >> 21) & 01777;
  if (ptr[i] == 0)
  {
    ptr[i] = (w_t)  k_frame_alloc();
    if (ptr[i] == 0)
    {
      panic("L4 PANIC: reserved memory exhausted during mpt_insert\n");
    }
    for (j = 0, leaf_ptr = (mpt_leaf_t *) ptr[i];
	 j < 256; j++)
    {
      leaf_ptr[j].pte0 = leaf_ptr[j].pte1 =
	INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
	EL_Cache_CNWb |  EL_Valid | EL_Global;
    }
  }
  
  
  leaf_ptr = (mpt_leaf_t *) ptr[i];
  i = (vaddr >> 13) & 0377;
  return &leaf_ptr[i];
}

extern char outbuff[];

void mpt_print(as_t *a)
{
  unsigned short i1, i2, i3, i4, i5, i6;
  mpt_t *p1, *p2, *p3, *p4, *p5;
  mpt_leaf_t *p6;
  unsigned int leaf, entries, guard;
  entries = leaf = guard = 0;
  p1 = (mpt_t *) ((udw_t)a - 3);
  for (i1 = 0;
#ifdef TLB_INSTR       
       i1 < 4;
#else
        i1 < 8;
#endif
       i1++,entries++)  /* 61-63 */
  {
    if (p1[i1] != 0)
    {
      guard++;
      p2 = (mpt_t *) p1[i1];
      for (i2 = 0; i2 < 1024; i2++,entries++) /* 51-60 */
      {
	if (p2[i2] != 0)
	{
	  guard++;
	  p3 = (mpt_t *) p2[i2];
	  for (i3 = 0; i3 < 1024; i3++,entries++) /* 41 - 50 */
	  {
	    if (p3[i3] != 0)
	    {
	      guard++;
	      p4 = (mpt_t *) p3[i3];
	      for (i4 = 0; i4 < 1024; i4++,entries++) /* 31 - 40 */
	      {
		if (p4[i4] != 0)
		{
		  guard++;
		  p5 = (mpt_t *) p4[i4];
		  for (i5 = 0; i5 < 1024; i5++,entries++) /* 21 - 30 */
		  {
		    if (p5[i5] != 0)
		    {
		      guard++;
		      p6 = (mpt_leaf_t *) p5[i5];
		      for (i6 = 0; i6 < 256; i6++) /* 13 - 20 */
		      {
			entries += 2;
			if (p6[i6].pte0 != 0)
			{
			  leaf++;
#ifdef SIM			  
			  printf("vaddr = 0x%llx pte = 0x%x\n",
				 (udw_t) i1 << 61 |
				 (udw_t) i2 << 51 |
				 (udw_t) i3 << 41 |
				 i4 << 31 |
				 i5 << 21 | i6 << 13, p6[i6].pte0);
#else
#ifndef TLB_INSTR			  
			  sprintf(outbuff,"vaddr = 0x%llx pte = 0x%x\n\r",
				 (udw_t) i1 << 61 |
				 (udw_t) i2 << 51 |
				 (udw_t) i3 << 41 |
				 i4 << 31 |
				 i5 << 21 | i6 << 13, p6[i6].pte0 << 6);
			  printbuff();
#endif			  
#endif			  
			}
			
			if (p6[i6].pte1 != 0)
			{
			  leaf++;
#ifdef SIM			  
			  printf("vaddr = 0x%llx pte = 0x%x\n",
				 (udw_t) i1 << 61 |
				 (udw_t) i2 << 51 |
				 (udw_t) i3 << 41 |
				 i4 << 31 |
				 i5 << 21 | i6 << 13 | 1 << 12, p6[i6].pte1);
#else
#ifndef TLB_INSTR			  
			  sprintf(outbuff,"vaddr = 0x%llx pte = 0x%x\n\r",
				 (udw_t) i1 << 61 |
				 (udw_t) i2 << 51 |
				 (udw_t) i3 << 41 |
				 i4 << 31 |
				 i5 << 21 | i6 << 13 | 1 << 12, p6[i6].pte1
				  << 6);
			  printbuff();
#endif
#endif			  
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
#ifdef TLB_INSTR  
  sprintf(outbuff, "entries = %d leaf = %d  guard = %d\n\r",
	  entries, leaf, guard);
  printbuff();

  sprintf(outbuff, "depth sum = %d\n\r",
	  0);
  printbuff();
#endif
}
    
void vm_map(tcb_t *snd_tcb, dword_t snd_fpage, dword_t snd_cp,
	    tcb_t *rcv_tcb, dword_t rec_fpage)
{
  
  udw_t snd_base ,end, rec_base, map_size;
  uw_t read_write_mask;
  
  
#ifdef KDEBUG
  alphn_set("vmap");
#endif	  
   
  {
    udw_t snd_size_mask;
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
      return;
    }
    
    if (rec_base + map_size > MAXUSEG)
    {
      return;
    }
    
    end = snd_base + map_size - 1;
    
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

  if (map_size == FRAMESIZE)
  {
    mpt_leaf_t *dst;
    pte_t *src_pte;
    
    /* do single page case */
    src_pte = vm_lookup_pte(snd_tcb->gpt_pointer, snd_base);
    if (src_pte != 0 && (*src_pte & EL_Valid))
    {
      dst = mpt_insert(rcv_tcb, rec_base);
      if (rec_base & FRAMESIZE)
      {
	mpt_map(rcv_tcb, rec_base, &dst->mt_ptr1,  &dst->pte1,
		*src_pte, *(w_t *) ((char *)src_pte + 8), read_write_mask);
      }
      else
      {
	mpt_map(rcv_tcb, rec_base, &dst->mt_ptr0,  &dst->pte0,
		*src_pte, *(w_t *) ((char *)src_pte + 8), read_write_mask);
      }
    }
  }
  else
  {
    /* now walk down snd tree */
    unsigned short i1, i2, i3, i4, i5, i6;
    unsigned short s1, s2, s3, s4, s5, s6;
    unsigned short e1, e2, e3, e4, e5, e6;
    mpt_t *p1, *p2, *p3, *p4, *p5;
    mpt_leaf_t *p6;
    int started = 0;

    
    p1 = (mpt_t *) ((udw_t)snd_tcb->gpt_pointer - 3);
    s1 = snd_base >> 61;
    e1 = end >> 61;
    if (s1 != e1)
    {
      started = 1;
    }
    for (i1 = s1; i1 <= e1; i1++)  /* 61-63 */
    {
      if (p1[i1] != 0)
      {
	p2 = (mpt_t *) p1[i1];
	if (started)
	{
	  s2 = 0; e2 = 1023;
	}
	else
	{
	  s2 = (snd_base >> 51) & 01777;
	  e2 = (end >> 51) & 01777;
	  if (s2 != e2)
	  {
	    started = 1;
	  }
	}
	for (i2 = s2; i2 <= e2; i2++) /* 51-60 */
	{
	  if (p2[i2] != 0)
	  {
	    p3 = (mpt_t *) p2[i2];
	    if (started)
	    {
	      s3 = 0; e3 = 1023;
	    }
	    else
	    {
	      s3 = (snd_base >> 41) & 01777;
	      e3 = (end >> 41) & 01777;
	      if (s3 != e3)
	      {
		started = 1;
	      }
	    }
	    for (i3 = s3; i3 <= e3; i3++) /* 41 - 50 */
	    {
	      if (p3[i3] != 0)
	      {
		p4 = (mpt_t *) p3[i3];
		if (started)
		{
		  s4 = 0; e4 = 1023;
		}
		else
		{
		  s4 = (snd_base >> 31) & 01777;
		  e4 = (end >> 31) & 01777;
		  if (s4 != e4)
		  {
		    started = 1;
		  }
		}
		for (i4 = s4; i4 <= e4; i4++) /* 31 - 40 */
		{
		  if (p4[i4] != 0)
		  {
		    p5 = (mpt_t *) p4[i4];
		    if (started)
		    {
		      s5 = 0; e5 = 1023;
		    }
		    else
		    {
		      s5 = (snd_base >> 21) & 01777;
		      e5 = (end >> 21) & 01777;
		      if (s5 != e5)
		      {
			started = 1;
		      }
		    }
		    for (i5 = s5; i5 <= e5; i5++) /* 21 - 30 */
		    {
		      if (p5[i5] != 0)
		      {
			p6 = (mpt_leaf_t *) p5[i5];
			if (started)
			{
			  s6 = 0; e6 = 255;
			}
			else
			{
			  s6 = (snd_base >> 13) & 0377;
			  e6 = (end >> 13) & 0377;
			  if (s6 != e6 || (map_size > 4096))
			  {
			    started = 1;
			  }
			}
			for (i6 = s6; i6 <= e6; i6++) /* 21 - 30 */
			{
			  w_t *mt_ptr;
			  uw_t *pte;
			  mpt_leaf_t *dst;
			  if (started)
			  {
			    if ((p6[i6].pte0 & EL_Valid ||
				 p6[i6].pte1 & EL_Valid))
			    {
			      dst = mpt_insert(rcv_tcb, rec_base);
			      
			      mpt_map(rcv_tcb,
				      rec_base,
				      &dst->mt_ptr0,
				      &dst->pte0,
				      p6[i6].pte0,
				      p6[i6].mt_ptr0,
				      read_write_mask);
			      
			      mpt_map(rcv_tcb,
				      rec_base + FRAMESIZE,
				      &dst->mt_ptr1,
				      &dst->pte1,
				      p6[i6].pte1,
				      p6[i6].mt_ptr1,
				      read_write_mask);
			    }
			  }
			  rec_base += 2* FRAMESIZE;
			}
		      }
		      else
		      {
			rec_base += 1 << 21;
		      }
		    }
		  }
		  else
		  {
		    rec_base += 1 << 31;
		  }
		}
	      }
	      else
	      {
		rec_base += 1ul << 41;
	      }
	    }
	  }
	  else
	  {
	    rec_base += 1ul << 51;
	  }
	}
      }
      else
      {
	rec_base += 1ul << 61;
      }
    }
  }
}


void mpt_free_tree(mpt_t *mpt)
{
  unsigned short i1, i2, i3, i4, i5;
  mpt_t *p1, *p2, *p3, *p4, *p5;
  
  p1 = (mpt_t *) ((udw_t)mpt - 3);
  for (i1 = 0; i1 < 8; i1++)  /* 61-63 */
  {
    if (p1[i1] != 0)
    {
      p2 = (mpt_t *) p1[i1];
      for (i2 = 0; i2 < 1024; i2++) /* 51-60 */
      {
	if (p2[i2] != 0)
	{
	  p3 = (mpt_t *) p2[i2];
	  for (i3 = 0; i3 < 1024; i3++) /* 41 - 50 */
	  {
	    if (p3[i3] != 0)
	    {
	      p4 = (mpt_t *) p3[i3];
	      for (i4 = 0; i4 < 1024; i4++) /* 31 - 40 */
	      {
		if (p4[i4] != 0)
		{
		  p5 = (mpt_t *) p4[i4];
		  for (i5 = 0; i5 < 1024; i5++) /* 21 - 30 */
		  {
		    if (p5[i5] != 0)
		    {
		      k_frame_free((void *)p5[i5]);
		    }
		  }
		  k_frame_free((void *)p4[i4]);
		}
	      }
	      k_frame_free((void *)p3[i3]);
	    }
	  }
	  k_frame_free((void *)p2[i2]);
	}
      }
      k_frame_free((void *)p1[i1]);
    }
  }
  k_frame_free((void *)p1);
}

void vm_fpage_unmap(tcb_t *tcb, dword_t fpage, dword_t mask)
{
  ub_t map_size;
  udw_t base, end;
  uw_t read_write_mask;
  
 /* first decode args and check they are valid */
  
  map_size =  ((fpage & GPT_FPAGE_SIZE_MASK) >> GPT_FPAGE_SIZE_SHIFT);
  
  if (map_size < 12) map_size = 12;
  if (map_size > 63) map_size = 63;
  
  base = fpage & ~((1ul << map_size) -1);
  
  if (base >=  MAXUSEG) return;
  
  end =  base + (1ul << map_size) -1;
  if (end > MAXUSEG) end = MAXUSEG;

   /* now walk down snd tree */

#ifdef _KDEBUG
  sprintf(outbuff,"unmap: base 0x%llx end 0x%llx\r\n",
	  base, end);
  printbuff();
#endif

  if (map_size == FRAME_SIZE_SHIFT)
  {
    pte_t *pte;

    pte = vm_lookup_pte(tcb->gpt_pointer,base);
    if (pte != 0)
    {
      mpt_unmap(pte,
		(mt_node_t *)(*(w_t *) ((char *)pte + 8)),
		read_write_mask,
		tcb, mask);
    }
  }
  else
  {
    
    unsigned short i1, i2, i3, i4, i5, i6;
    unsigned short s1, s2, s3, s4, s5, s6;
    unsigned short e1, e2, e3, e4, e5, e6;
    mpt_t *p1, *p2, *p3, *p4, *p5;
    mpt_leaf_t *p6;
    int started = 0;

    
    p1 = (mpt_t *) ((udw_t)tcb->gpt_pointer - 3);
    s1 = base >> 61;
    e1 = end >> 61;
    if (s1 != e1)
    {
      started = 1;
    }
    for (i1 = s1; i1 <= e1; i1++)  /* 61-63 */
    {
      if (p1[i1] != 0)
      {
	p2 = (mpt_t *) p1[i1];
	if (started)
	{
	  s2 = 0; e2 = 1023;
	}
	else
	{
	  s2 = (base >> 51) & 01777;
	  e2 = (end >> 51) & 01777;
	  if (s2 != e2)
	  {
	    started = 1;
	  }
	}
	for (i2 = s2; i2 <= e2; i2++) /* 51-60 */
	{
	  if (p2[i2] != 0)
	  {
	    p3 = (mpt_t *) p2[i2];
	    if (started)
	    {
	      s3 = 0; e3 = 1023;
	    }
	    else
	    {
	      s3 = (base >> 41) & 01777;
	      e3 = (end >> 41) & 01777;
	      if (s3 != e3)
	      {
		started = 1;
	      }
	    }
	    for (i3 = s3; i3 <= e3; i3++) /* 41 - 50 */
	    {
	      if (p3[i3] != 0)
	      {
		p4 = (mpt_t *) p3[i3];
		if (started)
		{
		  s4 = 0; e4 = 1023;
		}
		else
		{
		  s4 = (base >> 31) & 01777;
		  e4 = (end >> 31) & 01777;
		  if (s4 != e4)
		  {
		    started = 1;
		  }
		}
		for (i4 = s4; i4 <= e4; i4++) /* 31 - 40 */
		{
		  if (p4[i4] != 0)
		  {
		    p5 = (mpt_t *) p4[i4];
		    if (started)
		    {
		      s5 = 0; e5 = 1023;
		    }
		    else
		    {
		      s5 = (base >> 21) & 01777;
		      e5 = (end >> 21) & 01777;
		      if (s5 != e5)
		      {
			started = 1;
		      }
		    }
		    for (i5 = s5; i5 <= e5; i5++) /* 21 - 30 */
		    {
		      if (p5[i5] != 0)
		      {
			p6 = (mpt_leaf_t *) p5[i5];
			if (started)
			{
			  s6 = 0; e6 = 255;
			}
			else
			{
			  s6 = (base >> 13) & 0377;
			  e6 = (end >> 13) & 0377;
			}
			for (i6 = s6; i6 <= e6; i6++) /* 21 - 30 */
			{
			  mpt_unmap(&p6[i6].pte0,
				    (mt_node_t *) p6[i6].mt_ptr0,
				    read_write_mask,
				    tcb, mask);
			  mpt_unmap(&p6[i6].pte1,
				    (mt_node_t *) p6[i6].mt_ptr1,
				    read_write_mask,
				    tcb, mask);
			  
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}



void vm_tcb_unmap(as_t *as, dword_t base)
{
  udw_t end;
  end =  base + (MAX_THREADS * TCB_SIZE) ;
  {
    
    unsigned short i1, i2, i3, i4, i5, i6;
    unsigned short s6;
    unsigned short e6;
    mpt_t *p1, *p2, *p3, *p4, *p5;
    mpt_leaf_t *p6;

    
    p1 = (mpt_t *) ((udw_t)as - 3);
    i1 = base >> 61;
    if (p1[i1] != 0)
    {
      p2 = (mpt_t *) p1[i1];
      i2 = (base >> 51) & 01777;
      if (p2[i2] != 0)
      {
	p3 = (mpt_t *) p2[i2];
	i3 = (base >> 41) & 01777;
	if (p3[i3] != 0)
	{
	  p4 = (mpt_t *) p3[i3];
	  i4 = (base >> 31) & 01777;
	  if (p4[i4] != 0)
	  {
	    p5 = (mpt_t *) p4[i4];
	    i5 = (base >> 21) & 01777;
	    if (p5[i5] != 0)
	    {
	      p6 = (mpt_leaf_t *) p5[i5];
	      s6 = (base >> 13) & 0377;
	      e6 = (end >> 13) & 0377;
	      for (i6 = s6; i6 <= e6; i6++) /* 21 - 30 */
	      {
		if (((p6[i6].pte0 & EL_PFN) << 6) != INVALID_TCB_BASE)
		{
#ifdef KDEBUG
		  if ((((p6[i6].pte0 & EL_PFN) << 6) == 0))
		  {
		    panic("L4 PANIC: found invalid PTE in tcb space"); 
		  }
#endif	    
	      
		  tcb_frame_free((void *)base,
				 (void *) ((p6[i6].pte0 & EL_PFN) << 6));
		  p6[i6].pte0 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
		    EL_Cache_CNWb |  EL_Valid | EL_Global;
		  
		  tlb_sync_shared(base);
		}
		base += FRAMESIZE;
		if (((p6[i6].pte1 & EL_PFN) << 6) != INVALID_TCB_BASE)
		{
#ifdef KDEBUG
		  if ((((p6[i6].pte1 & EL_PFN) << 6) == 0))
		  {
		    panic("L4 PANIC: found invalid PTE in tcb space"); 
		  }
#endif	    
		  
		  tcb_frame_free((void *)base,
				 (void *) ((p6[i6].pte1 & EL_PFN) << 6));
		  p6[i6].pte1 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
		    EL_Cache_CNWb |  EL_Valid | EL_Global;
		  
		  tlb_sync_shared(base);
		}
		base += FRAMESIZE;
	      }
	    }
	  }
	}
      }
    }
  }
}

void mpt_map(tcb_t *rcv_tcb, udw_t dst_addr, w_t *mt_ptr, uw_t *pte,
	     uw_t src_pte, w_t src_mt_ptr, uw_t read_write_mask)
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
      *pte =  src_pte & read_write_mask;
      tlb_sync(rcv_tcb, dst_addr, *pte);
    }
    /* else we already have a mapping, leave as is */
  }
}

void mpt_unmap(pte_t *pte, mt_node_t *start, udw_t read_write_mask,
	       tcb_t *tcb, dword_t mask)
{
  
  if ((pte != 0) && (*pte & EL_Valid))
  {
    mt_node_t *x;
    void *t;
    /* have to unmap children of pager */
    
    x = start;
    if (mask & L4_FPAGE_RW_MASK)
    {
      while (x != start || x->child != 0)
      {
	mpt_t * gpt_ptr;
	while (x->child != 0)
	{
	  x = (mt_node_t *) x->child;
	}
	((mt_node_t *)x->parent)->child = x->sister;
	
	/* invalidate gpt */
#ifndef SIM		
	gpt_ptr =* (mpt_t **)
	  (((udw_t)(x->task)
	    << 36 >> 46 <<  TCB_SIZE_SHIFT)
	   + TCB_VBASE + T_GPT_POINTER);
#else
	gpt_ptr = vas[x->task].tcb.gpt_pointer;
#endif		  
	t = (void *)
	  vm_lookup_pte(gpt_ptr, x->vaddr);
	if (t != 0)
	{
	  *(pte_t *)t = 0;
#ifdef SIM		      
	  tlb_sync(&vas[x->task].tcb, x->vaddr, 0);
#else
	  tlb_sync((tcb_t *)
		   (((udw_t)(x->task)
		     << 36 >> 46 <<
		     TCB_SIZE_SHIFT)
		    + TCB_VBASE), x->vaddr, 0);
#endif		    
	}
	else
	{
	  panic("L4 PANIC: PTE not found, page table corrupt?"); /* ok */
	}
	
	t = (void *)x;
	x = (mt_node_t *) x->parent;
	mt_mem_free((mt_node_t *)t);
	
      }
    }
    else
    {
      read_write_mask = ~ EL_Dirty;
      
      do {
	while (x->child != 0)
	  x = (mt_node_t *) x->child;
	while (x != start)
	{
	  mpt_t * gpt_ptr;
#ifndef SIM		  
	  gpt_ptr =
	    * (mpt_t **)
	    (((udw_t)(x->task)
	      << 36 >> 46 <<  TCB_SIZE_SHIFT)
	     + TCB_VBASE + T_GPT_POINTER);
#else
	  gpt_ptr = vas[x->task].tcb.gpt_pointer;
#endif		  
	  t = (void *) vm_lookup_pte(gpt_ptr,
				     x->vaddr);
	  if (t != 0)
	  {
	    (*(pte_t *)t) &= read_write_mask;
#ifdef SIM			
	    tlb_sync(&vas[x->task].tcb, x->vaddr,
		     (*(pte_t *)t));
#else
	    tlb_sync((tcb_t *)
		     (((udw_t)(x->task)
		       << 36 >> 46 <<
		       TCB_SIZE_SHIFT)
		      + TCB_VBASE),
		     x->vaddr,(*(pte_t *)t));
	    
#endif			
	  }
	  else
	  {
	    panic("L4 PANIC: PTE not found, page table corrupt?"); /* ok */
	  }
	  
	  if (x->sister != 0)
	  {
	    x = (mt_node_t *) x->sister;
	    break;
	  }
	  else
	  {
	    x = (mt_node_t *) x->parent;
	  }
	}
      } while (x != start);
    }
    
    if ((dw_t)mask < 0)
    {
      if (mask & L4_FPAGE_RW_MASK)
      {
	*pte = 0;
#ifdef SIM		    
	tlb_sync(&vas[start->task].tcb, start->vaddr, 0);
#else
	tlb_sync(tcb, start->vaddr, 0);
#endif
	
	/* remove start as well */
	if ( ((mt_node_t *)((mt_node_t *)start->parent)->child)
	     == start)
	{
	  ((mt_node_t *)start->parent)->child = start->sister;
	}
	else
	{
	  t =(void  *) ((mt_node_t *)start->parent)->child;
	  while((mt_node_t *)((mt_node_t *)t)->sister != start)
	  {
	    t = (void *)(((mt_node_t *) t)->sister);
	  }
	  
	  ((mt_node_t *)t)->sister = start->sister;
	}
	mt_mem_free(start);
      }
      
      else
      {
	read_write_mask = ~ EL_Dirty;
	*pte &= read_write_mask;
#ifdef SIM		    
	tlb_sync(&vas[start->task].tcb, start->vaddr,
		 *pte);
#else
	tlb_sync(tcb, start->vaddr, *pte);
#endif		    
      }
    }
  }
}
