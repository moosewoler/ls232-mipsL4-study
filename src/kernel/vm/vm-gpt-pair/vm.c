/****************************************************************************
 *      $Id: vm.c,v 1.4 1998/01/22 05:48:45 kevine Exp $
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
#include "gpt.h"

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
  
  gpt_mem_init();
  mt_mem_init();
  gpt_leaf_mem_init();
}




void vm_tcb_insert(as_t *a, dword_t v, dword_t paddr)
{
  gpt_leaf_t *l;
  
  l = gpt_insert(a, v);
  
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
  void *sp;
  ub_t s1, s2, level,size;
  unsigned short i;
  udw_t vaddr, end;
  gpt_t *gpt, *gptr;

  gpt = (gpt_t *) ((udw_t) as - 15);
  sp = alloca((WORDLEN/GPTSIZE+1)*8);
  end =  base + (MAX_THREADS * TCB_SIZE) ;
  
  /* now wander down the tree to where we start doing real work */
  
  vaddr = 0;
  i = base >> (WORDLEN-GPTROOTSIZE);
  level = WORDLEN;
  size = GPTROOTSIZE;
  
  
  do {
    gptr = &(gpt[i]);
    if (gptr->guard == NULL_GUARD(((udw_t)level),
				   ((udw_t)size),
				   ((udw_t)i)))
    {
      i++;
      break;
    }
    if (
	
	(
	 (~(udw_t)i & ((1ul << size) - 1ul)) 
	 == 
	 (
	  (gptr->guard >> (level - size)) & 
	  ((1ul << size) - 1ul)
	  )
	 ) ||
	(
	 ((1ul << size) + i)
	 ==
	 ((gptr->guard >> (level - size))
	  & ((1ul << (size + 1)) - 1ul))
	 )
	
	)
      
    {
      break;
    }
    else
    {
      s1 = gptr->guard & 077;
      s2 = WORDLEN-level+size;
      vaddr = (level == WORDLEN) ?
	((udw_t)i << (level - size)) |
	(gptr->guard >> s1 << s1 << s2 >> s2)
	: (vaddr >> level << level) | ((udw_t)i << (level - size)) |
	(gptr->guard >> s1 << s1 << s2 >> s2);
      push_pos(gpt,i+1,level,size,&sp);
      size = GPTSIZE;
      level = gptr->guard & 077;
      gpt = (gpt_t *)(gptr->u.ptr - 15);
      i = (base >> (level - size)) & (( 1ul << size) - 1);
    }
    
  } while (1);
  
  /* now we are near the beginning so we walk through the tree unmapping
     from here */

  push_pos(gpt,i,level,size,&sp);
  
  do
  {
    
    pop_pos(&gpt,&i,&level,&size,&sp);
    while  (i < (1 << size))
    {
      gptr = &(gpt[i]);
      if (gptr->guard == NULL_GUARD(((udw_t)level),
				    ((udw_t)size),
				    ((udw_t)i)))
      {
	i++;
	if (
	    (vaddr = (vaddr >> level << level) |
	     ((udw_t)i << (level - size))) > end)
	{
	  break;
	}
      }
      else
      {
	s1 = gptr->guard & 077;
	s2 = WORDLEN-level+size;
	vaddr = (level == WORDLEN) ?
	  ((udw_t)i << (level - size)) |
	  (gptr->guard >> s1 << s1 << s2 >> s2)
	  : (vaddr >> level << level) | ((udw_t)i << (level - size)) |
	  (gptr->guard >> s1 << s1 << s2 >> s2);
	
	if (vaddr >= end) break;
	
	if (
	    
	    (
	     (~(udw_t)i & ((1ul << size) - 1ul)) 
	     == 
	     (
	      (gptr->guard >> (level - size)) & 
	      ((1ul << size) - 1ul)
	      )
	     ) ||
	    (
	     ((1ul << size) + i)
	     ==
	     ((gptr->guard >> (level - size))
	      & ((1ul << (size + 1)) - 1ul))
	     )
	    
	    )
	{
	  if (vaddr >= base)
	  {
	    /* safe to assume both pte's are to be invalidated */
	    
	    if (((gptr->u.lf->pte0 & EL_PFN) << 6) != INVALID_TCB_BASE)
	    {
#ifdef KDEBUG
	      if (((gptr->u.lf->pte0 & EL_PFN) << 6) == 0)
	      {
		panic("L4 PANIC: found invalid PTE in tcb space"); /* ok */
	      }
#endif	    
	      
	      tcb_frame_free((void *)vaddr,
			     (void *) ((gptr->u.lf->pte0 & EL_PFN) << 6));
	      gptr->u.lf->pte0 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
		EL_Cache_CNWb |  EL_Valid | EL_Global;
	      
	      tlb_sync_shared(vaddr);
	    }
	    
	    if (((gptr->u.lf->pte1 & EL_PFN) << 6) != INVALID_TCB_BASE)
	    {
#ifdef KDEBUG	    
	      if (((gptr->u.lf->pte1 & EL_PFN) << 6) == 0)
	      {
		panic("L4 PANIC: found invalid PTE in tcb space"); /* ok */
	      }
#endif	    
	      tcb_frame_free((void *)(vaddr+FRAMESIZE),
			     (void *) ((gptr->u.lf->pte1 & EL_PFN) << 6));
	      gptr->u.lf->pte1 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
		EL_Cache_CNWb |  EL_Valid | EL_Global;
	      
	      tlb_sync_shared(vaddr+FRAMESIZE);
	    }
	  }
	  i++;
	}
	else
	{
	  push_pos(gpt,i+1,level,size,&sp);
	  level = gptr->guard & 077;
	  gpt = (gpt_t *)(gptr->u.ptr - 15);
	  i = 0;
	  size = GPTSIZE;
	}
      }
    } 
  } while (vaddr < end && level != WORDLEN);
}






void vm_sigz_insert_pte(dword_t vaddr, pte_t pte)
{
  gpt_leaf_t *l;
  kernel_vars *k;
  mt_node_t *tree_ptr;
  
  k = KERNEL_BASE;
  
  l = gpt_insert(k->sigz_tcb, vaddr);
  
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


void vm_print_fpage(as_t *a, dword_t v, dword_t s)
{
  gpt_print(a);
}


