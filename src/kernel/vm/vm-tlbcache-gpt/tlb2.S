/****************************************************************************
 *      $Id: tlb2.S,v 1.1 1998/12/14 02:14:56 gernot Exp $
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
#include "gptdefs.h"

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
	sd	t0, K_TLB_T0_SAVE(REGK0)
	sd	t1, K_TLB_T1_SAVE(REGK0)
	dmfc0	REGK0, C0_ENTRYHI
	lui	REGK1, TLB2_BASE
#else
	dla	REGK0, c0_ehi
	ld	REGK0, (REGK0)
	dla	REGK1, tlb2_base_ptr
	ld	REGK1, (REGK1)
#endif
	
	dsll	t0, REGK0, 39
	dsrl	t0, t0, 47
	daddu	REGK1, t0, REGK1

	ld	t0, (REGK1)
	ld	t1, 16(REGK1)

	beq	t0, REGK0, 1f
	lwu	t0, 8(REGK1)
	bne	t1, REGK0, 2f
	lwu	t0, 24(REGK1)
	b	3f
	lwu	t1, 28(REGK1)
1:	lwu	t1, 12(REGK1)


3:
#ifndef SIM	
	dmtc0	t0, C0_ENTRYLO0
	dmtc0	t1, C0_ENTRYLO1
	lui	REGK0, KERNEL_BASE
	tlbwr
	ld	t0, K_TLB_T0_SAVE(REGK0)
	ld	t1, K_TLB_T1_SAVE(REGK0)
	eret

2:	j	tlb2_miss
	nop

#else
	sw	t0, (a0)
	sw	t1, (a1)
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


PROC(tlb2_miss)
#ifndef	SIM
	.set	noreorder
	.set	noat
	/* pseudo randomly pick upper or lower entry in bucket */
	mfc0	t0, C0_COUNT
	lui	REGK0, KERNEL_BASE
	andi	t0, t0, 16
	daddu	REGK1, t0, REGK1

	/* free some more registers */
	sd	t2, K_TLB_T2_SAVE(REGK0)
	sd	t3, K_TLB_T3_SAVE(REGK0)
	sd	t8, K_TLB_T8_SAVE(REGK0)
	
	ld	t0, K_GPT_POINTER(REGK0) /* t0 = P */
	ori	t0, t0, 15
	dmfc0	t2, C0_BADVADDR       /* t2 = v */
#else
	/* pick pseudo random bucket an leave in REGK1 */
	/* virtual address in t2 */
	/* gpt_pointer | 15 in t0 */
	dla	t0, c0_count
	lw	t0, (t0)
	andi	t0, t0, 16
	daddu	REGK1, t0, REGK1

	dla	t0, sim_kern
	ld	t0, K_GPT_POINTER(t0);
	ori	t0, t0, 15
	dla	t2, c0_bva
	ld	t2, (t2)
#endif	

	
	dli	t1, WORDLEN - 4 - GPTROOTSIZE	/* t1 = r2 */
						/* REGK0 = r1 */
	.set noreorder

1:	/* 64 bit GPT parsing loop */
	dsrlv	t1, t2, t1
	or	t0, t1

	andi	t8, REGK0, 077 /* remember previous s1 */

	ld	REGK0, -15(t0)

	daddiu	t3, t0, -7 /* remember pointer to this level */

	ld	t0, -7(t0)
	xor	t2, REGK0
	dsrl    t1, t2, REGK0
	beql    t1, zero, 1b
	dsrl    t1, REGK0, 6

	/* test if match or page fault */	
	dsrl	t2, REGK0, 6
	dsllv	t2, t1, t2
	
#ifndef SIM	
	bne	t2, zero, xtlb_refill_fail
	dmfc0	t2, C0_BADVADDR
#else
	dla	a5, c0_bva
	bne	t2, zero, xtlb_refill_fail
	ld	t2, (a5)
#endif
		
	/* check if pair or single entry */
	daddiu	REGK0, t8, -(12 + GPTSIZE)
	bne	REGK0, zero, 2f /* single */
	andi	t2, t2, FRAMESIZE
	beq	t2, zero, 1f
	dsrl	t0, t0, 32
	/* pair odd */
	move	t1, t0
	b	3f
	lwu	t0, -16(t3)

1:	/* pair even */	
	b	3f
	lwu	t1, 16(t3)

2:	/* single */
	beq	t2, zero, 1f /* single even */
	move	t1, zero
	/* single odd */
	dsrl	t1, t0, 32 
	b	3f
	move	t0, zero

1:	/* single even */
	dsrl	t0, t0, 32
	
	/* okay we have a hit to load in tlb2 and TLB */

3:	
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
	ld	t3, K_TLB_T3_SAVE(REGK0)
	ld	t8, K_TLB_T8_SAVE(REGK0)
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
	ld	t2, K_TLB_T2_SAVE(REGK0)
	ld	t3, K_TLB_T3_SAVE(REGK0)
	j	fail_tlb_rfl_ent
	ld	t8, K_TLB_T8_SAVE(REGK0)
#else
	jr	ra
	move	v0, zero
#endif		
	.set reorder
END(xtlb_refill_fail)

	
PROC(tlb2_sync)
	/* called by C with args
	a0   vaddr
	a1   pte
	a2   dthrd
	*/
	.set	noreorder
#ifndef SIM	
	/* preserve current asid */
	dmfc0	a4, C0_ENTRYHI
	tid2tcb(a2, a3)
	ld	a2, T_ASID(a3)
	lui	t0, TLB2_BASE
#else
	dla	t0, tcb_sizeof
	ld	t0, (t0)
	dla	a3, vas
2:	beq	a2, zero, 1f
	nop
	daddu	a3, a3, t0
	daddiu	a2, a2, -1
	b	2b
	nop
1:	ld	a2, T_ASID(a3)	
	dla	t0, tlb2_base_ptr
	ld	t0, (t0)
#endif
	dli	t1, ~(8192-1)
	and	t1, a0, t1
	or	t3, t1, a2
	
#ifndef SIM
	/* flush the entry out of the tlb */
	dmtc0	t3, C0_ENTRYHI
	nop
	tlbp
	nop
	mfc0	t2, C0_INDEX
	bltz	t2, 1f
	andi	t2, a0, 4096
	tlbr
	beq	t2, zero, 2f
	nop
	dmtc0	a1, C0_ENTRYLO1
	b	3f
	nop
2:	dmtc0	a1, C0_ENTRYLO0
	nop
	nop
3:	tlbwi
	
1:	/* flush tlb2 */	

	/* build an index */
	dmtc0	a4, C0_ENTRYHI
#endif	
	dsll    t1, t1, 39     /* 128 k tlb2 with 32 byte buckets */
        dsrl    t1, t1, 47
        daddu   t1, t1, t0
	
	ld      t0, (t1)
	ld      t2, 16(t1)
	
	beq     t0, t3, 1f
	andi	a0, a0, 4096
	bne     t2, t3, 2f
	nop
	
	/* hit in bucket 2 */
	beq	a0, zero, 3f
	nop
	
	jr	ra
	sw	a1, 28(t1)
	
3:	
	jr	ra
	sw	a1, 24(t1)
	
1:	/* hit in bucket 1*/
        beq     a0, zero, 3f
	nop
	
        jr      ra
        sw      a1, 12(t1)
3:      
	jr	ra
	sw      a1, 8(t1)
	
2:	/* miss */
	jr	ra
	nop
	.set	reorder
END(tlb2_sync)

PROC(tlb2_sync_shared)
	/* called by C with args
	a0   vaddr
	a1   pte
	*/
	.set noreorder

#ifndef SIM
	lui	t0, TLB2_BASE
	dmfc0	a4, C0_ENTRYHI
#else
	dla	t0, tlb2_base_ptr
	ld	t0, (t0)
#endif
		
	dli	t8, ~(8192-1)
	and	t3, a0, t8
	
#ifndef SIM	
	dmtc0	t3, C0_ENTRYHI
	nop
	tlbp
	nop
	mfc0	t2, C0_INDEX
	bltz	t2, 1f
	andi	t2, a0, 4096
	tlbr
	beq	t2, zero, 2f
	nop
	dmtc0	a1, C0_ENTRYLO1
	b	3f
	nop
2:	dmtc0	a1, C0_ENTRYLO0
	nop
	nop
3:	tlbwi
	
1:	/* flush tlb2 */	
	dmtc0	a4, C0_ENTRYHI
#endif
	/* build an index */
        dsll    t1, t3, 39     /* 128 k tlb2 with 32 byte buckets */
        dsrl    t1, t1,	47
        daddu   t1, t1, t0
	
	ld      t0, (t1)
	ld      t2, 16(t1)
	and	t0, t0, t8
	and	t2, t2, t8
	bne     t0, t3, 1f
	andi	a0, a0, 4096
	
	/* hit in bucket 1 */
	beq	a0, zero, 3f
	nop
	
	b	1f
	sw	a1, 12(t1)
	
3:	
	b	1f
	sw	a1, 8(t1)
	
1:	bne	t2, t3, 2f
	nop
	/* hit in bucket 2*/
        beq     a0, zero, 3f
	nop
	
        jr      ra
        sw      a1, 28(t1)
3:      
	jr	ra
	sw      a1, 24(t1)
	
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
	sd	zero, -16(t0)
	sd	zero, -8(t0) 
1:	bne	t0, t1, 2b
	ld	AT, (t0)

	jr	ra
	nop
	.set	reorder
	.set	at
END(tlb2_flush_asid)


PROC(vm_initial_as)
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
	ld	t0, 16(s0)
	ld	t1, 24(s0)
#elif (GPTROOTSIZE == 4)
	ld	t0, 12*16(s0)
	ld	t1, 12*16+8(s0)
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

PROC(vm_new_as)
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
	sd	t0, 16(v0)
	sd	t1, 24(v0)
#elif (GPTROOTSIZE == 4)
	sd	t0, 12*16(v0)
	sd	t1, 12*16+8(v0)
#endif

	ld	ra, (sp)
	ld	s0, 8(sp)
	daddiu	sp, sp, 16
	jr	ra
END(vm_new_as)


	
PROC(vm_delete_as)
	daddiu	sp, sp, -8
	sd	ra, (sp)
	move	a1, a0
	/* remove shared branch of tree in dying task*/
	
	ld	a0, T_GPT_POINTER(a1)
#if (GPTROOTSIZE == 1)
	sd	zero, 16(a0) /* null guard */
	sd	zero, 24(a0)
#elif (GPTROOTSIZE == 4)
	dli	t0, 3 << 60
	sd	t0, 12*16(a0)
	sd	zero, 12*16+8(a0)
#endif

	jal	gpt_free_tree
	ld	ra, (sp)
	daddiu	sp, sp, 8
	jr	ra
END(vm_delete_as)

PROC(push_pos)
	.frame	sp, 0, ra
	ld	t0, (a3)
	sw	a0, (t0)
	sb	a1, 4(t0)
	sb	a2, 5(t0)
	daddiu	t0, t0, 8
	sd	t0, (a3)
	jr	ra
END(push_pos)

PROC(pop_pos)
	.frame	sp, 0, ra
	ld	t0, (a3)
	daddiu	t0, t0, -8
	sd	t0, (a3)	
	lw	t2, (t0)
	sd	t2, (a0)
	lbu	t2, 4(t0)
	sb	t2, (a1)
	lbu	t2, 5(t0)
	sb	t2, (a2)
	jr	ra
END(pop_pos)
