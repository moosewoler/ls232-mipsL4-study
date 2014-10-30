/****************************************************************************
 *      $Id: as.S,v 1.1 1998/12/14 02:14:17 gernot Exp $
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
#include "gpt.h"

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

	jal	gpt_init

	move	s0, v0
	move	a0, v0   /* gpt pointer */
	dli	a1, TCB_VBASE
	dli	a2, NIL_TCB_BASE

	jal	vm_tcb_insert
	
	/* also insert a dummy mapping at end of tcb vaddress space
	to ensure tree splits below top of gpt so top does not 
	change later */

	move	a0, s0
	dli	a1, TCB_VBASE + (MAX_TASKS * MAX_THREADS * TCB_SIZE)
	dli	a2, NIL_TCB_BASE

	jal	vm_tcb_insert
	
	/* now save the appropriate entry from tree */
	/* to be added to new tcbs to share tree */
	/* FIXME: handle different gpt root sizes */
#if (GPTROOTSIZE == 1)
	ld	t0, 16-15(s0)
	ld	t1, 24-15(s0)
#elif (GPTROOTSIZE == 2)
	ld	t0, 3*16-15(s0)
	ld	t1, 3*16+8-15(s0)
#elif (GPTROOTSIZE == 3)
	ld	t0, 6*16-15(s0)
	ld	t1, 6*16+8-15(s0)
#elif (GPTROOTSIZE == 4)
	ld	t0, 12*16-15(s0)
	ld	t1, 12*16+8-15(s0)
#endif
#ifndef SIM	
	lui	t2, KERNEL_BASE
#else
	dla	t2, sim_kern
#endif		
	sd	t0, K_TCB_GPT_GUARD(t2)
	sd	t1, K_TCB_GPT_POINTER(t2)

#ifndef SIM
	dli	t3, TCB_VBASE
#else
	dla	t3, vas
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

	jal	gpt_init
	sd	v0, T_GPT_POINTER(s0)

	/* FIXME add shared branch in tree */
#ifndef SIM	
	lui     a2, KERNEL_BASE
#else
	dla	a2, sim_kern
#endif		
	ld	t0, K_TCB_GPT_GUARD(a2)
	ld	t1, K_TCB_GPT_POINTER(a2)

#if (GPTROOTSIZE == 1)
	sd	t0, 16-15(v0)
	sd	t1, 24-15(v0)
#elif (GPTROOTSIZE == 2)
	sd	t0, 3*16-15(v0)
	sd	t1, 3*16+8-15(v0)
#elif (GPTROOTSIZE == 3)
	sd	t0, 6*16-15(v0)
	sd	t1, 6*16+8-15(v0)
#elif (GPTROOTSIZE == 4)
	sd	t0, 12*16-15(v0)
	sd	t1, 12*16+8-15(v0)
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
#if (GPTROOTSIZE == 1)
	sd	zero, 16-15(a0) /* null guard */
	sd	zero, 24-15(a0)
#elif (GPTROOTSIZE == 2)
	sd	zero, 3*16-15(a0)
	sd	zero, 3*16+8-15(a0)
#elif (GPTROOTSIZE == 3)
	dli	t0, 1 << 61
	sd	t0, 6*16-15(a0)
	sd	zero, 6*16+8-15(a0)
#elif (GPTROOTSIZE == 4)
	dli	t0, 3 << 60
	sd	t0, 12*16-15(a0)
	sd	zero, 12*16+8-15(a0)
#endif

	jal	gpt_free_tree
	ld	ra, (sp)
	daddiu	sp, sp, 8
	jr	ra
END(vm_delete_as)


PROC(push_pos)
	.frame	sp, 0, ra
	ld	t0, (a4)
	sw	a0, (t0)
	sh	a1, 4(t0)
	sb	a2, 6(t0)
	sb	a3, 7(t0)
	daddiu	t0, t0, 8
	sd	t0, (a4)
	jr	ra
END(push_pos)

PROC(pop_pos)
	.frame	sp, 0, ra
	ld	t0, (a4)
	daddiu	t0, t0, -8
	sd	t0, (a4)	
	lw	t2, (t0)
	sd	t2, (a0)
	lhu	t2, 4(t0)
	sh	t2, (a1)
	lbu	t2, 6(t0)
	sb	t2, (a2)
	lbu	t2, 7(t0)
	sb	t2, (a3)
	jr	ra
END(pop_pos)
