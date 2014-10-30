/****************************************************************************
 *      $Id: tlb2-1way-128.S,v 1.1 1998/12/14 02:15:10 gernot Exp $
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

#define TLB2_SIZE (128*1024)
	
#ifdef SIM
#include <sim.h>	
#define REGK0   a6
#define REGK1   a7
#else
#define REGK0   k0
#define REGK1   k1
#endif

	.data
tlb_msg:
	.asciiz "tlb refill miss"
tlb_refill_msg:
	.asciiz "tlb refill hit"

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
	lui	REGK1, TLB2_BASE
	dmfc0	REGK0, C0_ENTRYHI
#else
	dla	REGK0, c0_ehi
	ld	REGK0, (REGK0)
	dla	REGK1, tlb2_base_ptr
	ld	REGK1, (REGK1)
#endif

	dsll	t0, REGK0, 38
	dsrl	t0, t0, 47
	daddu	REGK1, t0, REGK1

	ld	t0, (REGK1)

	bne	t0, REGK0, 2f
	lwu	t0, 8(REGK1)
	lwu	REGK0, 12(REGK1)

3:
#ifndef SIM
# ifdef TLB_INSTR
	dmtc0	t0, C0_ENTRYLO0
	dmtc0	REGK0, C0_ENTRYLO1
	lui	REGK0, KERNEL_BASE
	tlbwr
	ld	t0, K_TLB_T0_SAVE(REGK0)
	mfc0	REGK1, C0_COUNT
j	tlb_instr
	nop
# else	
	dmtc0	t0, C0_ENTRYLO0
	dmtc0	REGK0, C0_ENTRYLO1
	lui	REGK0, KERNEL_BASE
	tlbwr
	
	ld	t0, K_TLB_T0_SAVE(REGK0)
	eret
# endif
2:	j	tlb2_miss
	lui	t0, KERNEL_BASE
#else
	sw	t0, (a0)
	sw	REGK0, (a1)
	li	v0, 1
	jr	ra
	nop

2:	b	tlb2_miss
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
	subu	REGK1, REGK1, t8
	ld	t8, K_TLB_MISS_TIME(REGK0)
	daddu	t8, t8, REGK1
	sd	t8, K_TLB_MISS_TIME(REGK0)
	ld	t8, K_TLB_T8_SAVE(REGK0)
	eret
	.set reorder
END(tlb_instr)	
#endif

	

PROC(tlb2_miss)
/****************************************************************************
 * we get here if the tlb cache misses 
 *
 * t0, t1 are usable, REGK0 contains a the vaddr of a pair, REGK1 a pointer
 * to the bucket in the cache for replacement.
 ****************************************************************************/
	.set	noreorder
	.set	noat
	
#ifndef	SIM
	/* free some more registers */
	sd	t1, K_TLB_T1_SAVE(t0)
	sd	t2, K_TLB_T2_SAVE(t0)
	ld	t2, K_GPT_POINTER(t0) /* t0 = P */
	dmfc0	REGK0, C0_ENTRYHI
#else
	/* pick pseudo random bucket an leave in REGK1 */
	/* virtual address in t2 */
	/* gpt_pointer | 15 in t0 */

	dla	t0, sim_kern
	ld	t2, K_GPT_POINTER(t0);
#endif	
	/* REGK1 -  pointer to bucket to replace */
	/* REGK0 - ehi */
	/* t2 - gpt pointer */
	/* t0, t1 usable */
	
	dli t1, WORDLEN - 4 - GPTROOTSIZE

1:	/* 64 bit GPT parsing loop */
	dsrlv	t1, REGK0, t1
	or	t2, t1
	ld	t0, -15(t2)
	ld	t2, -7(t2)
	xor	REGK0, t0
	dsrl     t1, REGK0, t0
	beql    t1, zero, 1b
	dsrl     t1, t0, 6

	/* test if match or page fault */	
	dsrl	REGK0, t0, 6
	dsllv	t1, t1, REGK0
	bne	t1, zero, xtlb_refill_fail
	nop
	/* valid match */
	lw	t0, (t2)
	lw	t1, 4(t2)
	
#ifndef SIM
	dmfc0	t2, C0_ENTRYHI
	sw	t0, 8(REGK1)
	sw	t1, 12(REGK1)
	sd	t2, (REGK1)

	dmtc0	t0, C0_ENTRYLO0
	dmtc0	t1, C0_ENTRYLO1
	lui	REGK0, KERNEL_BASE
	tlbwr
	ld	t0, K_TLB_T0_SAVE(REGK0)
	ld	t1, K_TLB_T1_SAVE(REGK0)
	ld	t2, K_TLB_T2_SAVE(REGK0)
#ifdef TLB_INSTR
	mfc0	REGK1, C0_COUNT
	subu	REGK1, REGK1, t8
	ld	t8, K_TLB_MISS_TIME(REGK0)
	daddu	t8, t8, REGK1
	sd	t8, K_TLB_MISS_TIME(REGK0)
	ld	t8, K_TLB2_MISS(REGK0)
	daddiu	t8, t8, 1
	sd	t8, K_TLB2_MISS(REGK0)
	ld	t8, K_TLB_T8_SAVE(REGK0)
#else	
	nop	/* avoid a potential 48 instruction routine */	
#endif
	/* trace(etms) */
	eret
#else
	dla	t2, c0_ehi
	ld	t2, (t2)
	sw	t0, 8(REGK1)
	sw	t1, 12(REGK1)
	sd	t2, (REGK1)

	sw	t0, (a0)
	sw	t1, (a1)

	dli	v0, 1
	jr	ra
	nop
#endif
	.set	at
	.set	reorder
END(tlb2_miss)



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
	ld	t8, K_TLB2_MISS(REGK0)
	daddiu	t8, t8, 1
	sd	t8, K_TLB2_MISS(REGK0)
	ld	t8, K_TLB_T8_SAVE(REGK0)
#endif
	
	j	fail_tlb_rfl_ent
	ld	t2, K_TLB_T2_SAVE(REGK0)
#else
	sw	zero, (a0)
	sw	zero, (a1)

	jr	ra
	move	v0, zero
#endif		
	.set reorder
END(xtlb_refill_fail)

	
PROC(tlb2_sync)
	/* called by C with args
	a0   tcb
	a1   vaddr
	a2   pte
	*/
	.set	noreorder
#ifndef SIM	
	/* preserve current asid */

	/* put ASID in a3 and TLB cache base in t0 */
	ld	a3, T_ASID(a0)
	dmfc0	a4, C0_ENTRYHI
	bgez	a3, 1f
	lui	t0, TLB2_BASE

	/* get ASID from task, not thread */
	dsrl	t1, a0, 18
	dsll	t1, t1, 18
	ld	a3, T_ASID(t1)
	bgez	a3, 1f
	sd	a3, T_ASID(a0) /* store what ever valid for task in thread
				doesn't matter if it's invalid */
	
	/* if ASID not valid in this address space then no need
	to shoot down TLB of cache */
	jr	ra
	nop
#else
	/* we are in simulator so assume ASID always valid */

	ld	a3, T_ASID(a0)	
	dla	t0, tlb2_base_ptr
	ld	t0, (t0)
#endif
	/* build a tag in ENTRYHI format */	
1:	dli	t1, ~(8192-1)
	and	t1, a1, t1
	or	t3, t1, a3
	
#ifndef SIM
	/* flush the entry out of the tlb */
	dmtc0	t3, C0_ENTRYHI
	nop
	tlbp
	nop
	mfc0	t2, C0_INDEX
	bltz	t2, 1f
	andi	t2, a1, 4096
	tlbr
	beq	t2, zero, 2f
	nop
	dmtc0	a2, C0_ENTRYLO1
	b	3f
	nop
2:	dmtc0	a2, C0_ENTRYLO0
	nop
3:	tlbwi
	
1:
	/* restore current ASID */
	dmtc0	a4, C0_ENTRYHI
#endif
	/* flush entry in tlb2 */

	/* build an index */
	dsll    t1, t3, 38     /* 8 k tlb2 with 32 byte buckets */
        dsrl    t1, t1, 47
        daddu   t1, t1, t0
	
	ld      t0, (t1)
	
	bne     t0, t3, 2f
	andi	a1, a1, 4096
	
1:	/* hit in bucket 1*/
        beq     a1, zero, 3f
	nop
	
        jr      ra
        sw      a2, 12(t1)
3:      
	jr	ra
	sw      a2, 8(t1)
	
2:	/* miss */
	jr	ra
	nop
	.set	reorder
END(tlb2_sync)

PROC(tlb2_sync_shared)
	/* called by C with args
	a0   vaddr
	*/
	.set noreorder

	/* load tlb cache base into t0 */
#ifndef SIM
	lui	t0, TLB2_BASE
	dmfc0	a4, C0_ENTRYHI /* save ehi */
#else
	dla	t0, tlb2_base_ptr
	ld	t0, (t0)
#endif
	
	/* build a tag in ASID 0 */
			
	dli	t8, ~(8192-1)
	and	t3, a0, t8

	/* probe TLB (relying on global bit) an nail it if found */
#ifndef SIM	
	dmtc0	t3, C0_ENTRYHI
	nop
	tlbp
	nop
	mfc0	t2, C0_INDEX
	bltz	t2, 1f
	nop
	dmtc0	zero, C0_ENTRYHI
	dmtc0	zero, C0_ENTRYLO1
	dmtc0	zero, C0_ENTRYLO0
	nop
	tlbwi
	nop	
1:
	/* restore current ASID */
	dmtc0	a4, C0_ENTRYHI
#endif
	/* build an index */
        dsll    t1, t3, 38     /* 128 k tlb2 with 32 byte buckets */
        dsrl    t1, t1,	47
        daddu   t1, t1, t0

	/* load tags */
	ld      t0, (t1)
	
	and	t0, t0, t8 /* mask out ASID for global compare */
	
	bne     t0, t3, 2f
	nop	
	/* hit bucket 1 */
	jr	ra	
	sd	zero, 0(t1)
	
2:	/* miss */
	jr	ra
	nop
	.set	reorder
END(tlb2_sync_shared)


PROC(tlb_cache_init)
#ifndef SIM	
	dli	t0, TLB2_BASE64
	dli	t1, TLB2_BASE64 + TLB2_SIZE
#else
	dla	t0, tlb2_base_ptr
	ld	t0, (t0)
	dli	t1, TLB2_SIZE
	daddu	t1, t1, t0
#endif	

1:	sd	zero, (t0)
	daddiu	t0, t0, 8
	bne	t0, t1, 1b
	jr	ra
END(tlb_cache_init)

PROC(tlb2_flush_asid) /* uses AT, t0, t1, ra, preserves a0 */
	.globl	tlb_cache_flush_asid
tlb_cache_flush_asid:

	/* a0 has asid we wish to clean */
	.set	noat
	.set	noreorder
#ifndef SIM	
	dli	t0, TLB2_BASE64
	dli	t1, TLB2_BASE64 + TLB2_SIZE
#else
	dla	t0, tlb2_base_ptr
	ld	t0, (t0)
	dli	t1, TLB2_SIZE
	daddu	t1, t1, t0
#endif	


	
	ld	AT, (t0)
2:	andi	AT, AT, EH_ASID_MASK
	bne	AT, a0, 1f
	daddiu	t0, t0, 16
	sd	zero, -8(t0)
	sd	zero, -16(t0)
1:	bne	t0, t1, 2b
	ld	AT, (t0)

	jr	ra
	nop
	.set	reorder
	.set	at
END(tlb2_flush_asid)

