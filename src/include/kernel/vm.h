#ifndef _VM_H
#define _VM_H
/****************************************************************************
 * $Id: vm.h,v 1.5 1998/01/22 05:46:15 kevine Exp $
 * Copyright (C) 1997, 1998 Kevin Elphinstone, Univeristy of New South
 * Wales.
 *
 * This file is part of the L4/MIPS micro-kernel distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ****************************************************************************/

#ifdef _LANGUAGE_C

#include <l4/types.h>

#ifdef SIM
#include <sim.h>
#else
#include <kernel/kernel.h>
#endif


typedef word_t pte_t;
typedef void as_t;

/* debugger */

extern void vm_print_fpage(as_t *as, dword_t base, dword_t size);


/* task control block */

extern void vm_tcb_insert(as_t *as, dword_t vaddr, dword_t paddr);
extern void vm_tcb_unmap(as_t *as, dword_t base);


/* address space manipulation */
extern void vm_initial_as(void);
extern void vm_new_as(tcb_t *tcb);
extern void vm_delete_as(tcb_t *tcb);

extern pte_t *vm_lookup_pte(as_t *as, dword_t vaddr);
extern void vm_sigz_insert_pte(dword_t vaddr, pte_t pte);

extern void vm_map(tcb_t *snd_tcb, dword_t snd_fpage, dword_t snd_cp,
                   tcb_t *rcv_tcb, dword_t rcv_fpage); 
extern void vm_fpage_unmap(tcb_t *tcb, dword_t fpage, dword_t mask);


/* tlb manipulation */

extern void tlb_flush_window(dword_t addr);
extern void tlb_flush_asid(byte_t asid);
extern void tlb_sync(tcb_t *tcb, dword_t vaddr, pte_t pte);
extern void tlb_sync_shared(dword_t vaddr);


#ifdef TLB_CACHE
/* cache */

extern void tlb_cache_init(void);
extern void tlb_cache_sync(tcb_t *tcb, dword_t vaddr, pte_t pte);
extern void tlb_cache_sync_shared(dword_t vaddr, pte_t pte);
extern void tlb_cache_flush_asid(byte_t asid);/* uses AT, t0, t1, ra, preserves a0 */
#endif

/* memory alloc pools */
extern void vm_mem_init(void);


#ifdef SIM
/* simulator */
extern void vm_check_map_trees(void *);
extern int xtlb_refill(pte_t *lo0, pte_t *lo1);

#endif

#endif
#endif
