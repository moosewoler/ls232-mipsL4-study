/****************************************************************************
 *      $Id: asid.S,v 1.1 1998/12/29 05:22:09 gernot Exp $
 *      Copyright (C) 1997, 1998 Kevin Elphinstone, Univeristy of New South
 *      Wales.
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
#include "macros.h"


/****************************************************************************
 *
 * Strategy for ASID management
 *
 * + ASIDs are stored in TCBs.
 *
 * + if ASID negative, then it's invalid.
 *
 * + A task's ASID is stored in thread 0, other threads get this one on demand.
 *
 * + ASIDs are kept in a free list.
 *
 * + if free list is empty ASIDs are removed from a task via FIFO arrangment.
 *
 ****************************************************************************/

PROC(asid_init)
	trace(asdi)
	move	a4, ra /* asid_free doesn't use a3 */
	lui	a0, KERNEL_BASE

	/* init free list */
	sd	zero, K_FREE_ASID_LIST(a0)

	/* init fifo */
	dli	a1, R4K_MAX_ASID
	sd	a1, K_ASID_FIFO_COUNT(a0)

	/* fill free list */
	dli	a3, 1 /* starting asid, 0 reserved by L4 */
	dli	a2, R4K_MAX_ASID + 1
	
1:	move	a0, a3
	jal	asid_free

	daddiu	a3, a3, 1

	bne	a2, a3, 1b
	
	jr	a4
END(asid_init)

PROC(asid_free)
	/* a0 has asid */
	.set	noat
	dli	t0, -1
	beq	t0, a0, 1f
	lui	t0, KERNEL_BASE
	dli	AT, ASID_BASE
	dsll	a0, a0, 3 
	daddu	a0, a0, AT
	ld	AT, K_FREE_ASID_LIST(t0)
	sd	a0, K_FREE_ASID_LIST(t0)
	sd	AT, (a0)
1:	jr	ra
	.set	at
END(asid_free)

PROC(asid_alloc)
	/* a0 has tid of thread requesting */
	.set	noat
	lui	t0, KERNEL_BASE
	ld	v0, K_FREE_ASID_LIST(t0)
	beq	v0, zero, 1f
	ld	AT, (v0)
	sd	a0, (v0)
	sd	AT, K_FREE_ASID_LIST(t0)
	dsrl	v0, v0, 3
	andi	v0, v0, 0xff
	jr	ra
1:	
	dli	v0, -1
	jr	ra	
	.set	at
END(asid_alloc)

.data
asid_get_msg:	
	.asciiz "no free ASIDS"	
PROC(asid_get)
	trace(asdg)
	.set	noat
	/* sp is valid and AT is only reg we can use */
	/* sp is for task needs ASID */

	/* first switch to asid 0 so tlb refills use system ASID */
	.set	noreorder
	dmtc0	zero, C0_ENTRYHI
	.set	reorder
	
	/* reserve some more registers to use */
	daddiu	sp, sp, -40
	sd	t0, (sp)
	sd	t1, 8(sp)
	sd	v0, 16(sp)
	sd	a0, 24(sp)
	sd	ra, 32(sp)

	
	/* check if task already has asid */
	tcbtop(t0)
	ld	t1, T_MYSELF-TCBO(t0)
	dsrl	a0, t1, TID_TASK_SHIFT
	dsll	a0, a0, TID_TASK_SHIFT
	tid2tcb(a0, t1)
	.set	noat
	ld	v0, T_ASID(t1)
	bltz	v0, 1f
	
	/* yes it does, so simply use that */
	.set	noreorder
	dmtc0	v0, C0_ENTRYHI
	b	2f
	sd	v0,  T_ASID-TCBO(t0)
	.set	reorder
	
1:
	/* else try to asid_alloc */

	jal	asid_alloc /* nails t0, v0, AT, ra */
	bltz	v0, 3f
	
	/* okay we has an asid from the free list */
	tcbtop(t0)
	.set noreorder
	sd	v0, T_ASID(t1)
	dmtc0	v0, C0_ENTRYHI
	b	2f
	sd	v0, T_ASID-TCBO(t0)
	.set	reorder	
3:

	/* Hmmm, no ASIDs available, need to remove one from active task */
	trace(asgt)
	/* pick asid from fifo counter */
	lui	t0, KERNEL_BASE
	ld	ra, K_ASID_FIFO_COUNT(t0)
	daddiu	a0, ra, -1
	bne	a0, zero, 4f
	dli	a0, R4K_MAX_ASID
4:	sd	a0, K_ASID_FIFO_COUNT(t0)
	trace(hvas)
	/* ra now has ASID, get tid that has it */
	dli	t0, ASID_BASE
	dsll	v0, ra, 3
	daddu	t0, t0, v0
	ld	t1, (t0)
	tcbtop(a0)
	ld	a0, T_MYSELF-TCBO(a0)
	sd	a0, (t0)   /* store my tid as current holder of asid */

	dsrl	t1, t1, TID_TASK_SHIFT
	dsll	t1, t1, TID_TASK_SHIFT /* AT now has taskid of old holder */
	tid2tcb(t1, a0) /* a0 now has tcb of old holder */
	.set	noat
	trace(nlas)
	/* store invalid ASID in task and threads of old holder */
	dli	AT, -1
1:	sd	AT, T_ASID(a0)
	ld	a0, T_PRESENT_NEXT(a0)
	bne	a0, zero, 1b
	
			
	/* TLB flush */
	trace(tlbf)
	move	a0, ra
	jal	tlb_flush_asid  /* uses AT, t0, t1, ra, preserves a0 */

#ifdef  TLB_CACHE
	trace(tb2f)
	/* TLB2 flush */
	jal	tlb_cache_flush_asid /* uses AT, t0, t1, ra, preserves a0 */
#endif	
	/* assign asid to current thread and thread 0 */
	trace(asas)
	tcbtop(t0)
	ld	t1, T_MYSELF-TCBO(t0)
	dsrl	v0, t1, TID_TASK_SHIFT
	dsll	v0, v0, TID_TASK_SHIFT
	tid2tcb(v0, t1)
	.set	noat
	sd	a0, T_ASID(t1)
	.set	noreorder
	dmtc0	a0, C0_ENTRYHI
	sd	a0, T_ASID-TCBO(t0)
	.set	reorder
	
2:
	ld	t0, (sp)
	ld	t1, 8(sp)
	ld	v0, 16(sp)
	ld	a0, 24(sp)
	ld	ra, 32(sp)
	daddiu	sp, sp, 40
	jr	ra
	.set	at
END(asid_get)
