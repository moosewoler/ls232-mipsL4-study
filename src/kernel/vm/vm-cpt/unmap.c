/****************************************************************************
 *      $Id: unmap.c,v 1.1 1998/03/19 23:40:33 kevine Exp $
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

void cpt_unmap(pte_t *pte, mt_node_t *mt_ptr, udw_t read_write_mask,
	       tcb_t *tcb, dword_t mask)
{
  if ((pte != 0) && (*pte & EL_Valid))
  {
    mt_node_t *start, *x;
    void *t;
#ifdef _SIM		  
    printf("vm_fpage_unmap: pte 0x%x\n",
	   *pte);
#endif		  
    /* have to unmap children of pager */
    start = mt_ptr;
    
    x = start;
    if (mask & L4_FPAGE_RW_MASK)
    {
      while (x != start || x->child != 0)
      {
	cpt_t * cpt_ptr;
	while (x->child != 0)
	{
	  x = (mt_node_t *) x->child;
	}
	((mt_node_t *)x->parent)->child = x->sister;
	
	/* invalidate gpt */
#ifndef SIM		
	cpt_ptr =* (cpt_t **) (((udw_t)(x->task)
				<< 36 >> 46 <<  TCB_SIZE_SHIFT)
			       + TCB_VBASE + T_GPT_POINTER);
#else
	cpt_ptr = vas[x->task].tcb.gpt_pointer;
#endif		  
	cpt_remove(cpt_ptr, x->vaddr);
#ifdef SIM		      
	tlb_sync(&vas[x->task].tcb, x->vaddr, 0);
#else
	tlb_sync((tcb_t *)
		 (((udw_t)(x->task)
		   << 36 >> 46 <<  TCB_SIZE_SHIFT)
		  + TCB_VBASE), x->vaddr, 0);
#endif		    
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
	  cpt_t * cpt_ptr;
#ifndef SIM		  
	  cpt_ptr =* (cpt_t **) (((udw_t)(x->task)
				  << 36 >> 46 <<  TCB_SIZE_SHIFT)
				 + TCB_VBASE + T_GPT_POINTER);
#else
	  cpt_ptr = vas[x->task].tcb.gpt_pointer;
#endif		  
	  t = (void *) vm_lookup_pte(cpt_ptr, x->vaddr);
	  if (t != 0)
	  {
	    (*(pte_t *)t) &= read_write_mask;
#ifdef SIM			
	    tlb_sync(&vas[x->task].tcb, x->vaddr, (*(pte_t *)t));
#else
	    tlb_sync((tcb_t *)
		     (((udw_t)(x->task)
		       << 36 >> 46 <<  TCB_SIZE_SHIFT)
		      + TCB_VBASE), x->vaddr,(*(pte_t *)t));
	    
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
	cpt_remove(tcb->gpt_pointer,start->vaddr);
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
	tlb_sync(&vas[start->task].tcb, start->vaddr, *pte);
#else
	tlb_sync(tcb, start->vaddr, *pte);
#endif		    
      }
    }
  }
}
