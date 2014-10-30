/****************************************************************************
 *      $Id: lookup.S,v 1.1 1998/12/14 02:14:39 gernot Exp $
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
#include <kernel/machine.h>	
#include "ipt.h"


PROC(vm_lookup_pte)
	.frame	sp, 0, ra	
	/* a0 gpt ptr	
	 * a1 vaddr
	 */
	.set noreorder
	/* use direct hash into 128K array, 32 byte slots */
	dsrl	t1, a1, 13     /* t1 now has tag (vaddr >> 13) */
#ifdef IPT_128	
	dsll	t0, t1, 52
	dsrl	t0, t0, 47
#elif  defined(IPT_8)
	dsll	t0, t1, 56
	dsrl	t0, t0, 51
#endif		
	daddu	t0, t0, a0

	/* t0  now has pointer to bucket */

	ld	t2, (t0)
	andi	a7, a1, FRAMESIZE
	bne	t2, t1, 1f
	dsrl	a7, a7, 10
	daddiu	a7, a7, 8   /* a7 offset to odd/even pair */
	jr	ra
	daddu	v0, t0, a7

1:	ld	t0, 16(t0)   /* follow links to end of list */
	beq	t0, zero, 2f /* found null link */
	nop
	ld	t2,(t0)      /* load tag from link */
	bne	t2, t1, 1b   /* compare */
	nop
	
	daddiu	a7, a7, 8   /* a7 offset to odd/even pair */
	jr	ra
	daddu	v0, t0, a7

2:      /* page fault */
	j	ra
	dli	v0, 0
	
	.set reorder
END(vm_lookup_pte)

PROC(ipt_lookup_pair)
	.frame	sp, 0, ra	
	/* a0 gpt ptr	
	 * a1 vaddr
	 */
	.set noreorder
	/* use direct hash into 128K array, 32 byte slots */
	dsrl	t1, a1, 13     /* t1 now has tag (vaddr >> 13) */
#ifdef IPT_128	
	dsll	t0, t1, 52
	dsrl	t0, t0, 47
#elif  defined(IPT_8)
	dsll	t0, t1, 56
	dsrl	t0, t0, 51
#endif		
	daddu	t0, t0, a0

	/* t0  now has pointer to bucket */

	ld	t2, (t0)
	bne	t2, t1, 1f
	move	v0, t0
	jr	ra
	nop

1:	ld	t0, 16(t0)   /* follow links to end of list */
	beq	t0, zero, 2f /* found null link */
	nop
	ld	t2,(t0)      /* load tag from link */
	bne	t2, t1, 1b   /* compare */
	move	v0, t0
	jr	ra
	nop

2:      /* page fault */
	j	ra
	dli	v0, 0
	
	.set reorder
END(ipt_lookup_pair)
	
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
	dmfc0	REGK0, C0_ENTRYHI
#else
	dla	REGK0, c0_ehi
	ld	REGK0, (REGK0)
	dla	REGK1, sim_kern
	ld	REGK1,  K_GPT_POINTER(REGK1)
#endif

#ifdef IPT_128	
	dsll	t0, REGK0, 39      /* form index */
	dsrl	t0, t0, 47
#elif defined(IPT_8)
	dsll	t0, REGK0, 43      /* form index */
	dsrl	t0, t0, 51
#endif 		
	daddu	REGK1, t0, REGK1   /* reg k1 has pointer */
	ld	t0, (REGK1)        /* reg t0 has tag from bucket */
	dsrl	REGK0, REGK0, 13   /* reg k0 has tag to match */
	
	bne 	t0, REGK0, 1f
	lwu	t0, 8(REGK1)
	lwu	REGK0, 12(REGK1)
#ifndef SIM	
	dmtc0	t0, C0_ENTRYLO0
	dmtc0	REGK0, C0_ENTRYLO1
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

1:	j	head_miss
	lui	t0, KERNEL_BASE
#else
	sw	t0, (a0)
	sw	REGK0, (a1)

	dli	v0, 1
	jr	ra
	nop	
	
1:	
	b	head_miss
	nop	
	
#endif		
	.set reorder
	.set at
xtlb_refill_end:
	.globl xtlb_refill_end
END(xtlb_refill)	

PROC(head_miss)
	.set noreorder
	.set noat
#ifndef SIM
	sd	t1, K_TLB_T1_SAVE(t0)
#endif	
	ld	t1, 16(REGK1)   /* follow links to end of list */
2:	beql	t1, zero, xtlb_refill_fail    /* found null link */
#ifndef SIM
	lui	t0, KERNEL_BASE
#else
	move	t1, zero /* just testing, should not matter */
#endif		
	ld	t0,(t1)         /* load tag from link */
	bnel	t0, REGK0, 2b   /* compare */
	ld	t1, 16(t1)

	                        /* REGK1 has head pointer */
				/* t1 has chain pointer */
				/* REGK0 tag */
				/* t0 free */

	ld	t0, (REGK1)     /* swap tags */
	sd	REGK0, (REGK1)
	sd	t0, (t1)
	
	ld	REGK0, 8(REGK1) /* swap ptes */
	ld	t0, 8(t1)
	sd	REGK0, 8(t1)
	sd	t0, 8(REGK1)

	ld	REGK0, 24(REGK1) /* swap mt ptrs */
	ld	t0, 24(t1)
	sd	REGK0, 24(t1)
	sd	t0, 24(REGK1)


	lwu	t0, 8(REGK1)
	lwu	REGK0, 12(REGK1)
#ifndef SIM
	dmtc0	t0, C0_ENTRYLO0
	dmtc0	REGK0, C0_ENTRYLO1
	lui	REGK0, KERNEL_BASE
	tlbwr
	ld	t0, K_TLB_T0_SAVE(REGK0)
	ld	t1, K_TLB_T1_SAVE(REGK0)
# ifdef TLB_INSTR
	mfc0	REGK1, C0_COUNT
	subu	REGK1, REGK1, t8
	ld	t8, K_TLB_MISS_TIME(REGK0)
	daddu	t8, t8, REGK1
	sd	t8, K_TLB_MISS_TIME(REGK0)
	ld	t8, K_TLB2_MISS(REGK0)
	daddiu	t8, t8, 1
	sd	t8, K_TLB2_MISS(REGK0)
	ld	t8, K_TLB_T8_SAVE(REGK0)
	nop /* to avoid routine ever being a multiple of 48 */
# endif	
	eret
	

#else
	sw	t0, (a0)
	sw	REGK0, (a1)
	li	v0, 1
	jr	ra
	nop

3:	b	xtlb_refill_fail
	nop	
#endif		
	.set reorder
	.set at
END(head_miss)	


PROC(xtlb_refill_fail)
	.set noreorder
	/* it might be a kernel page so try here */
#ifndef SIM
	ld	t1,  K_TLB_T1_SAVE(t0)
	lui	REGK1, TLB2_BASE
	dmfc0	REGK0, C0_ENTRYHI
#else
	dla	REGK0, c0_ehi
	ld	REGK0, (REGK0)
	dla	REGK1, tlb2_base_ptr
	ld	REGK1, (REGK1)
#endif
		
	dsll	t0, REGK0, 39      /* form index */
	dsrl	t0, t0, 47
	daddu	REGK1, t0, REGK1   /* reg k1 has pointer */
	ld	t0, (REGK1)        /* reg t0 has tag from bucket */
	dsrl	REGK0, REGK0, 13   /* reg k0 has tag to match */
	
	beq 	t0, REGK0, 2f
	lwu	t0, 8(REGK1)
	
1:	ld	REGK1, 16(REGK1)   /* follow links to end of list */
	beq	REGK1, zero, 3f /* found null link */
	nop
	ld	t0,(REGK1)      /* load tag from link */
	bne	t0, REGK0, 1b   /* compare */
	lwu	t0, 8(REGK1)
	
2:	lwu	REGK0, 12(REGK1)

#ifndef SIM		
	dmtc0	t0, C0_ENTRYLO0
	dmtc0	REGK0, C0_ENTRYLO1
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
#else
	sw	t0, (a0)
	sw	REGK0, (a1)
	li	v0, 1
	jr	ra
	nop
#endif	

#ifndef SIM	
3:	lui	REGK0, KERNEL_BASE

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
	ld	t0, K_TLB_T0_SAVE(REGK0)
#else
3:	sw	zero, (a0)
	sw	zero, (a1)

	jr	ra
	move	v0, zero
#endif		
	.set reorder
END(xtlb_refill_fail)


