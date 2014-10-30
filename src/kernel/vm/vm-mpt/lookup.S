/****************************************************************************
 *      $Id: lookup.S,v 1.1 1998/12/14 02:14:47 gernot Exp $
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
#include <kernel/kernel.h>
#include <kernel/machine.h>	
#include <r4kc0.h>
#include <asm.h>
#include <regdef.h>

#include "mpt.h"

PROC(vm_lookup_pte)
	.frame	sp, 0, ra	
	/* a0 gpt ptr	
	 * a1 vaddr
	 */
	.set noreorder

	dsrl	t0, a1, 59	/* index using bits 61 - 63 */
	or	a0, a0, t0
	lw	a0, -3(a0)
	dsrl	t0, a1, 49	/* index using bits 51 - 60 */
	beq	a0, zero, 2f
	andi	t0, t0, 07774
	or	a0, a0, t0
	lw	a0, (a0)
	dsrl	t0, a1, 39	/* index using bits 41 - 50 */
	beq	a0, zero, 2f
	andi	t0, t0, 07774
	or	a0, a0, t0
	lw	a0, (a0)
	dsrl	t0, a1, 29	/* index using bits 31 - 40 */
	beq	a0, zero, 2f
	andi	t0, t0, 07774
	or	a0, a0, t0
	lw	a0, (a0)
	dsrl	t0, a1, 19	/* index using bits 21 - 30 */
	beq	a0, zero, 2f
	andi	t0, t0, 07774
	or	a0, a0, t0
	lw	a0, (a0)
	dsrl	t0, a1, 9	/* index using bits 13 - 20 */
	beq	a0, zero, 2f
	andi	a1, a1, FRAMESIZE
	andi	t0, t0, 07760
	beq	a1, zero, 1f
	or	v0, t0, a0	
	daddiu	v0, v0, 4
1:	j	ra
	nop

2:      /* page fault */
	j	ra
	dli	v0, 0
	
	.set reorder
END(vm_lookup_pte)
#ifdef SIM
#include <sim.h>	
#define REGK0   a6
#define REGK1   a7
#else
#define REGK0   k0
#define REGK1   k1
#endif

	
PROC(xtlb_refill)
	.frame	sp, 0, ra	
xtlb_refill_start:
	.globl xtlb_refill_start
	.set noreorder
	.set noat
#ifndef SIM	
	lui	REGK0, KERNEL_BASE

#ifdef TLB_INSTR
	sd	t8, K_TLB_T8_SAVE(REGK0)
	ld	t8, K_TLB_MISS(REGK0)
	daddiu	t8, t8, 1
	sd	t8, K_TLB_MISS(REGK0)
	mfc0	t8, C0_COUNT
#endif	
	sd	t0, K_TLB_T0_SAVE(REGK0)
	ld	REGK1, K_GPT_POINTER(REGK0)
	j	xtlb_xtra
	dmfc0	REGK0, C0_BADVADDR
#else
	dla	REGK0, c0_bva
	ld	REGK0, (REGK0)
	dla	REGK1, sim_kern
	b	xtlb_xtra
	ld	REGK1,  K_GPT_POINTER(REGK1)
#endif
	.set reorder
	.set at
xtlb_refill_end:
	.globl xtlb_refill_end
END(xtlb_refill)

PROC(xtlb_xtra)
	.set noreorder
	.set noat

	/* REGK1 pointer */
	/* REGK0 vaddr */
	/* t0 work space */
	dsrl	t0, REGK0, 59	/* index using bits 61 - 63 */
	or	REGK1, REGK1, t0
	lw	REGK1, -3(REGK1)
	dsrl	t0, REGK0, 49	/* index using bits 51 - 60 */
	beq	REGK1, zero, 2f
	andi	t0, t0, 07774
	or	REGK1, REGK1, t0
	lw	REGK1, (REGK1)
	dsrl	t0, REGK0, 39	/* index using bits 41 - 50 */
	beq	REGK1, zero, 2f
	andi	t0, t0, 07774
	or	REGK1, REGK1, t0
	lw	REGK1, (REGK1)
	dsrl	t0, REGK0, 29	/* index using bits 31 - 40 */
	beq	REGK1, zero, 2f
	andi	t0, t0, 07774
	or	REGK1, REGK1, t0
	lw	REGK1, (REGK1)
	dsrl	t0, REGK0, 19	/* index using bits 21 - 30 */
	beq	REGK1, zero, 2f
	andi	t0, t0, 07774
	or	REGK1, REGK1, t0
	lw	REGK1, (REGK1)
	dsrl	t0, REGK0, 9	/* index using bits 13 - 20 */
	beq	REGK1, zero, 2f
	andi	t0, t0, 07760
	or	t0, t0, REGK1	
	lwu	REGK0, (t0)
	lwu	REGK1, 4(t0)
5:	

#ifndef SIM
	dmtc0	REGK0, C0_ENTRYLO0
	dmtc0	REGK1, C0_ENTRYLO1
	lui	REGK0, KERNEL_BASE
	tlbwr
	ld	t0, K_TLB_T0_SAVE(REGK0)
#ifdef TLB_INSTR	
	mfc0	REGK1, C0_COUNT
	subu	REGK1, REGK1, t8
	ld	t8, K_TLB_MISS_TIME(REGK0)
	daddu	t8, t8, REGK1
	sd	t8, K_TLB_MISS_TIME(REGK0)
	ld	t8, K_TLB_T8_SAVE(REGK0)
#endif	
	eret

2:	j	xtlb_refill_fail
	nop

#else
	sw	REGK0, (a0)
	sw	REGK1, (a1)
	li	v0, 1
	jr	ra
	nop

2:	b	xtlb_refill_fail
	nop	
#endif		

	.set reorder
	.set at
END(xtlb_xtra)

	
PROC(xtlb_refill_fail)
	.set noreorder
#ifndef SIM	
	lui	REGK0, KERNEL_BASE
	ld	t0, K_TLB_T0_SAVE(REGK0)
#ifdef TLB_INSTR
	mfc0	REGK1, C0_COUNT
	subu	REGK1, REGK1, t8
	ld	t8, K_TLB_MISS_TIME(REGK0)
	daddu	t8, t8, REGK1
	sd	t8, K_TLB_MISS_TIME(REGK0)
	ld	t8, K_TLB_T8_SAVE(REGK0)
#endif
	j	fail_tlb_rfl_ent
	nop
#else
	sw	zero, (a0)
	sw	zero, (a1)

	jr	ra
	move	v0, zero
#endif		
	.set reorder
END(xtlb_refill_fail)
