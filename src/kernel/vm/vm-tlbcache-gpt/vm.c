/****************************************************************************
 *      $Id: vm.c,v 1.3 1998/01/22 05:49:06 kevine Exp $
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
#include <kernel/vm.h>
#include "gptdefs.h"
#include <kernel/panic.h>

static gpt_leaf_t null_leaf = {0,0};
void  vm_tcb_insert(as_t *gpt, udw_t vaddr, udw_t paddr)
{
  
  gpt_leaf_t *leaf;
  
  /* first insert pte in gpt */
  
  leaf = gpt_lookup_ptr(gpt, vaddr);
  
  if (leaf == 0)
  {
    
    leaf = gpt_insert(gpt, vaddr, null_leaf);
    if (leaf == 0)
    {
      panic("L4 PANIC: failed to insert TCB PTE"); /* ok */
    }
  }
  /* insert global entry */
  leaf->pte = paddr >>  FRAME_SIZE_SHIFT << 6 |
    EL_Cache_CNWb |  EL_Valid | EL_Dirty | EL_Global;
  leaf->mt_ptr = 0;

  tlb2_sync_shared(vaddr, leaf->pte);
  
  /* ugly pairing of global bit here, so we must ensure there is an invalid
     global entry for pair */

  if (vaddr & FRAMESIZE)
  {
    vaddr -= FRAMESIZE;
  }
  else
  {
    vaddr += FRAMESIZE;
  }
  
  leaf = gpt_lookup_ptr(gpt, vaddr);
  if (leaf == 0)
  {
    
    leaf = gpt_insert(gpt, vaddr, null_leaf);
    if (leaf == 0)
    {
      panic("L4 PANIC: failed to insert second PTE of TCB PTE pair"); /* ok */
    }
  }
  
  if (!(leaf->pte & EL_Global))
  {
    leaf->pte =  INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
			      EL_Cache_CNWb |  EL_Valid | EL_Global;
    leaf->mt_ptr = 0;
    tlb2_sync_shared(vaddr, leaf->pte);
  }
}
   
void vm_tcb_unmap(as_t *as, dword_t base)
{
  task_tcb_unmap(base, (gpt_t *)as);
}


pte_t *vm_lookup_pte(as_t *as, dword_t vaddr)
{
  return &((gpt_lookup_ptr(as, vaddr))->pte);
}

void vm_mem_init(void)
{
  gpt_mem_init();
  mt_mem_init();
}

void vm_fpage_unmap(tcb_t *tcb, dword_t fpage, dword_t mask)
{
  gpt_fpage_unmap(fpage, mask, tcb->gpt_pointer);

}

extern void gpt_print_fpage(gpt_t *gpt, udw_t base, ub_t size);
void vm_print_fpage(as_t *as, dword_t base, dword_t size)
{
  if (size == 64)
  {
    gpt_print(as);
  }
  else
  {
    gpt_print_fpage((gpt_t *)as, base, size);
  }
}
     
void vm_sigz_insert_pte(dword_t vaddr, pte_t pte)
{
  kernel_vars *k = KERNEL_BASE;
  
  sigma0_pt_insert(k->sigz_tcb->gpt_pointer,  vaddr, pte);
}

void vm_map(tcb_t *snd_tcb, dword_t snd_fpage, dword_t snd_cp,
                   tcb_t *rcv_tcb, dword_t rcv_fpage)
{
  gpt_map(snd_tcb->gpt_pointer, snd_fpage, snd_cp,
	  rcv_tcb->gpt_pointer, rcv_fpage, rcv_tcb->myself);
  
}

void tlb_cache_sync(tcb_t *t, dword_t v, pte_t p)
{
  panic("L4 PANIC: tlb_cache_sync unimplemented");
}

void tlb_cache_sync_shared(dword_t v, pte_t p)
{
  panic("L4 PANIC: tlb_cache_sync unimplemented");
}
