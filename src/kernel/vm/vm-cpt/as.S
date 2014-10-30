/****************************************************************************
 *      $Id: as.S,v 1.1 1998/12/14 02:14:05 gernot Exp $
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

#include <asm.h>
#include <regdef.h>
#include <r4kc0.h>
#include <kernel/kernel.h>
#include <kernel/machine.h>
#include "ipt.h"

#ifdef SIM
#include <sim.h>	
#define REGK0   a6
#define REGK1   a7
#else
#define REGK0   k0
#define REGK1   k1
#endif


/****************************************************************************
 * void vm_initial_as(void): 
 *
 * desc:- set up the initial page table with a page table entry for the
 * first task control block. Also set up anything required for future
 * sharing of kernel task control blocks.
 *
 * side effect:- sets kernel and task gpt pointer to point to the initial
 * page table.
 *
 ****************************************************************************/
	
PROC(vm_initial_as)
	.frame sp, 16, ra
	.mask   0x80000000, -8
	daddiu	sp, sp, -16
	/* set up shared tcb space tree */
	/* insert mapping for tcb_vbase -> phys address */

	sd	s0, (sp)
	sd	ra, 8(sp)

	jal	tcb_ipt_init
	
	jal	cpt_init

	move	s0, v0
	move	a0, v0   /* gpt pointer */
	dli	a1, TCB_VBASE
	dli	a2, NIL_TCB_BASE

	jal	vm_tcb_insert
	
#ifndef SIM
        lui     t2, KERNEL_BASE
#else
        dla     t2, sim_kern
#endif
        sd      t0, K_TCB_GPT_GUARD(t2)
        sd      t1, K_TCB_GPT_POINTER(t2)

#ifndef SIM
        dli     t3, TCB_VBASE
#else
        dla     t3, vas
#endif
	sd	s0, K_GPT_POINTER(t2)
	sd	s0, T_GPT_POINTER(t3)   /* bootstrap initial tcb */ 

	ld	s0, (sp)
	ld	ra, 8(sp)
	
	daddiu	sp, sp, 16
	jr	ra
END(vm_initial_as)


/****************************************************************************
 * void vm_new_as(tcb_t *tcb)
 *
 * desc: creates a new page table for a new address space and associates it
 * with the tcb passed to it.
 ****************************************************************************/
	
PROC(vm_new_as)
	.frame	sp, 16, ra
	.mask   0x80000000, -16
	/* a0 tcb */
	daddiu	sp, sp, -16
	sd	ra, (sp)
	sd	s0, 8(sp)
	move	s0, a0

	jal	cpt_init
	sd	v0, T_GPT_POINTER(s0)
#ifdef TLB_INSTR
	dli	t0, CPT_INITIAL_SIZE
	sd	t0, T_PT_SIZE(s0)
	sd	zero, T_PT_NUMBER(s0)
#endif	
	ld	ra, (sp)
	ld	s0, 8(sp)
	daddiu	sp, sp, 16
	jr	ra
END(vm_new_as)

/****************************************************************************
 * void vm_delete_as(tcb_t *tcb)
 *
 * desc: Clean up any resources associated with the address space
 * associated with the tcb indicated.
 ****************************************************************************/

/* not called in SIM */
	
PROC(vm_delete_as)
	.frame  sp, 8, ra
	.mask   0x80000000, -8
	daddiu	sp, sp, -8
	sd	ra, (sp)
	move	a1, a0
	/* remove shared branch of tree in dying task*/
	
	ld	a0, T_GPT_POINTER(a1)

	jal	cpt_free
	ld	ra, (sp)
	daddiu	sp, sp, 8
	jr	ra
END(vm_delete_as)


