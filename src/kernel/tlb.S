/****************************************************************************
 *      $Id: tlb.S,v 1.1 1998/12/14 02:12:55 gernot Exp $
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
#include <kernel/vm.h>


PROC(tlb_flush_window)
	/* a0 has window address */
	.set	noreorder
	.set	noat
	dmfc0	a1, C0_ENTRYHI
	dsrl	a0, a0, RECV_WINDOW_SHIFT
	move	t0, zero
	li	t1, 48
	
2:	mtc0	t0, C0_INDEX
	nop
	tlbr
	dmfc0	AT, C0_ENTRYHI
	dsrl	AT, AT, RECV_WINDOW_SHIFT
	bne	AT, a0, 1f
	addiu	t0, t0, 1
	dmtc0	zero, C0_ENTRYLO0
	dmtc0	zero, C0_ENTRYLO1
	nop
	tlbwi
1:	bne	t0, t1, 2b	
	nop
	jr	ra
	dmtc0	a1, C0_ENTRYHI
	.set	reorder
	.set	at
END(tlb_flush_window)

PROC(tlb_flush_asid) /* uses AT, t0, t1, ra, preserves a0 */
	.set	noreorder
	.set	noat
	
	li	t0, 48
	dmfc0	t1, C0_ENTRYHI
	
2:	addiu	t0, t0, -1
	mtc0	t0, C0_INDEX
	nop
	tlbr
	dmfc0	AT, C0_ENTRYHI
	andi	AT, AT, EH_ASID_MASK
	bne	AT, a0, 1f
	nop
	dmtc0	zero, C0_ENTRYHI
	dmtc0	zero, C0_ENTRYLO0
	dmtc0	zero, C0_ENTRYLO1
	nop
	tlbwi
1:	bne	t0, zero, 2b	
	nop
	dmtc0	t1, C0_ENTRYHI
	jr	ra
	nop
	.set	reorder
	.set	at
END(tlb_flush_asid)

PROC(tlb_sync)
	/* a0 tcb
	a1 vaddr
	a2 pte
	*/
	.set	noreorder
	/* put ASID in a3  */
	ld	a3, T_ASID(a0)
	dmfc0   a7, C0_ENTRYHI
	bgez	a3, 1f
	
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


1:	dli	t1, ~(8192-1)
	and	t1, a1, t1	
	or	t3, t1, a3	
	
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
	nop
1:	
	dmtc0   a7, C0_ENTRYHI
	jr	ra
	nop
	.set	reorder
	
END(tlb_sync)

PROC(tlb_sync_shared)
	/* called by C with args
	a0   vaddr
	*/
	.set noreorder
	dmfc0	a7, C0_ENTRYHI

	dli	t8, ~(8192-1)
	and	t3, a0, t8
	
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
1:	dmtc0	a7, C0_ENTRYHI
	jr	ra
	nop
	.set	reorder	
END(tlb_sync_shared)


