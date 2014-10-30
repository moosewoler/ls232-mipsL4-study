/****************************************************************************
 *      $Id: lookup.S,v 1.1 1998/12/14 02:14:30 gernot Exp $
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
#include <regdef.h>
#include <asm.h>	
#include <kernel/kernel.h>
#include <r4kc0.h>
#include "gpt.h"


PROC(vm_lookup_pte)
	.frame	sp, 0, ra	
	/* a0 gpt ptr	
	 * a1 vaddr
	 */
	andi	a7, a1, FRAMESIZE
	dli t1, WORDLEN - 4 - GPTROOTSIZE

	.set noreorder
1:	/* 64 bit GPT parsing loop */
	dsrlv	t1, a1, t1
	or	a0, t1
	ld	t0, -15(a0)
	daddiu	v0, a0, -7
	ld	a0, -7(a0)
	xor	a1, t0
	dsrl     t1, a1, t0
	beql    t1, zero, 1b
	dsrl     t1, t0, 6

	/* test if match or page fault */	
	dsrl	a1, t0, 6
	dsllv	t1, t1, a1
	bne	t1, zero, 2f
	move	v0, a0
	/* valid match */
	bnel	a7, zero, 3f
	daddiu	v0, v0, 4
3:	j	ra
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
	sd	t1, K_TLB_T1_SAVE(REGK0)
	ld	REGK1, K_GPT_POINTER(REGK0)
	dmfc0	REGK0, C0_ENTRYHI
#else
	dla	REGK0, c0_ehi
	ld	REGK0, (REGK0)
	dla	REGK1, sim_kern
	ld	REGK1,  K_GPT_POINTER(REGK1)
#endif
	dli t1, WORDLEN - 4 - GPTROOTSIZE
	
	.set noreorder
1:	/* 64 bit GPT parsing loop */
	dsrlv	t1, REGK0, t1
	or	REGK1, t1
	ld	t0, -15(REGK1)
	ld	REGK1, -7(REGK1)
	xor	REGK0, t0
	dsrl     t1, REGK0, t0
	beql    t1, zero, 1b
	dsrl     t1, t0, 6

	/* test if match or page fault */	
	dsrl	REGK0, t0, 6
	dsllv	t1, t1, REGK0
	bne	t1, zero, 2f
	nop
	/* valid match */
	lw	t0, (REGK1)
	lw	t1, 4(REGK1)


#ifndef SIM	
#ifdef TLB_INSTR
	j	tlb_instr
	dmtc0	t0, C0_ENTRYLO0
#else
	dmtc0	t0, C0_ENTRYLO0
	dmtc0	t1, C0_ENTRYLO1
	lui	REGK0, KERNEL_BASE
	tlbwr
	
	ld	t0, K_TLB_T0_SAVE(REGK0)
	ld	t1, K_TLB_T1_SAVE(REGK0)
	
	eret
#endif
2:	j	xtlb_refill_fail
	nop

#else
	sw	t0, (a0)
	sw	t1, (a1)
	li	v0, 1
	jr	ra
	nop

2:	b	xtlb_refill_fail
	nop	
#endif		

	.set reorder
	.set at
xtlb_refill_end:
	.globl xtlb_refill_end
END(xtlb_refill)

#ifdef TLB_INSTR
PROC(tlb_instr)
	.set noreorder
	dmtc0	t1, C0_ENTRYLO1
	lui	REGK0, KERNEL_BASE
	tlbwr
	ld	t0, K_TLB_T0_SAVE(REGK0)
	ld	t1, K_TLB_T1_SAVE(REGK0)
	mfc0	REGK1, C0_COUNT
	subu	REGK1, REGK1, t8
	ld	t8, K_TLB_MISS_TIME(REGK0)
	daddu	t8, t8, REGK1
	sd	t8, K_TLB_MISS_TIME(REGK0)
	ld	t8, K_TLB_T8_SAVE(REGK0)
	
	eret
	.set reorder
END(tlb_instr)	
#endif
		
	
PROC(xtlb_refill_fail)
	.set noreorder
#ifndef SIM	
	lui	REGK0, KERNEL_BASE
	ld	t0, K_TLB_T0_SAVE(REGK0)
	ld	t1, K_TLB_T1_SAVE(REGK0)
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
