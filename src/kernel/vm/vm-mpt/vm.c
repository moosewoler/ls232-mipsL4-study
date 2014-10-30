/****************************************************************************
 *      $Id: vm.c,v 1.2 1998/01/22 05:48:55 kevine Exp $
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
#include "mpt.h"

void vm_mem_init(void)
{
  mt_mem_init();
}


void vm_tcb_insert(as_t *a, dword_t v, dword_t paddr)
{
  mpt_leaf_t *l;
  
  l = tcb_mpt_insert(a, v);
  
  if (v & FRAMESIZE)
  {
    l->pte1 = paddr >>  FRAME_SIZE_SHIFT << 6 |
      EL_Cache_CNWb |  EL_Valid | EL_Dirty | EL_Global;
  }
  else
  {
    l->pte0 = paddr >>  FRAME_SIZE_SHIFT << 6 |
      EL_Cache_CNWb |  EL_Valid | EL_Dirty | EL_Global;
  }
  tlb_sync_shared(v);
}

void vm_sigz_insert_pte(dword_t vaddr, pte_t pte)
{
  mpt_leaf_t *l;
  kernel_vars *k;
  mt_node_t *tree_ptr;
  
  k = KERNEL_BASE;
  
  l = mpt_insert(k->sigz_tcb, vaddr);
  
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
  mpt_print(a);
}
