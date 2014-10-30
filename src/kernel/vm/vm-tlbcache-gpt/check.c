/****************************************************************************
 *      $Id: check.c,v 1.3 1998/01/22 05:49:00 kevine Exp $
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
#include "gptdefs.h"
#include <r4kc0.h>
#include <kernel/vm.h>

#ifdef SIM
#include <sim.h>

extern sim_as_t vas[];

void vm_check_map_trees(void *ptr)
{
  mt_node_t *x, *start;
  udw_t count;
  pte_t *leaf;
  /* parse the tree to see if okay */
  leaf = vm_lookup_pte(vas[0].tcb.gpt_pointer, (dword_t)ptr);
  if (leaf != 0 && (*leaf & EL_Valid))
  {
    start = (mt_node_t *)  (* ((word_t *)((word_t)leaf + 4)));
    while (start->parent != 0)
    {
      start = (mt_node_t *) (start->parent);
    }
    x = start;
    
    count = 0;
    do {
      while (x->child != 0)
      {
	x = (mt_node_t *) x->child;
      }
      while (x != start)
      {
	
	/* do some checks */
	count++;
	leaf = vm_lookup_pte(vas[x->task].tcb.gpt_pointer, x->vaddr);
	if (leaf == 0 && vas[x->task].v_table[(x->vaddr) >> FRAME_SIZE_SHIFT])
	{
	  abort();
	}
	if ((leaf != 0) &&
	    ((*leaf & (EL_Valid|EL_Dirty)) !=
	     (vas[x->task].v_table[(x->vaddr) >> FRAME_SIZE_SHIFT]
	      & (EL_Valid|EL_Dirty))))
	{
	  abort();
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
#ifdef CHK_DEBUG
    printf("check_map_trees: frame %d has %d mappings\n",
	   i, count);
#endif
    
  }
}
#endif
