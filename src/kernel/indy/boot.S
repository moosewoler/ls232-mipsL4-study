/****************************************************************************
 * $Id: boot.S,v 1.1 1998/12/14 02:13:29 gernot Exp $
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
 ****************************************************************************/  /****************************************************************************
 *
 * Put the p4000 board into a known state in prep for installing our own
 * kernel.
 *
 * Follows te guidelines in "VR4000 Reset and Initialization Sequence" found at
 * http://www.mips.com/HTMLs/r4400_apps/Boot_In_bk/Boot_Init_cv.html
 *
 ****************************************************************************/
	
	
#include <regdef.h>
#include <kernel/p4000i.h>
#include <r4kc0.h>

	.ent __boot
	.globl __boot
__boot:
	/* Initialize GP registers (not needed, done in PMON) */

	/* Initialize status register (this was done in ROM reset 
	 * routine, but lets change a few things to take over the 
	 * machine from the ROM).
	 */
	.set noreorder

	
	li	t0,  EL_Cache_CNWb >> 3
	mtc0	t0, C0_CONFIG
	nop
	
	mfc0	t0, C0_STATUS
	li	t1, ST_DE | ST_KX | ST_UX | ST_SX
	or	t0, t1
zero_mask = ~(ST_CU0|ST_CU1|ST_CU2|ST_CU3|ST_IM|ST_ERL|ST_EXL|ST_IE)
	li	t1, zero_mask /* define const to avoid assembler complaints */
	and	t0, t1
	mtc0	t0, C0_STATUS
	nop
	
	/* Init Config register (this basically sets cache 
	 * coherency in KSEG0. Don't know which is best so leave
	 * as PMON set it).
	 */

	/* Init Cause (clear software int) */

	mfc0	t1, C0_CAUSE
	li	t0, ~(CA_IP0 | CA_IP1)	
	and	t1, t0
	mtc0	t1, C0_CAUSE
	
	/* Init FPU control reg (not using FPU so leave as set by PMON) */

	/* Init the caches (already done by PMON, this code is 
	 * running cached. Need to install cache error handling
	 * later).
	 */	

	/* Init TLB */

	move	t0, zero /* entry and entryhi */
	li	t1, 48
	mtc0	zero, C0_ENTRYLO0
	mtc0	zero, C0_ENTRYLO1
	mtc0	zero, C0_WIRED 
	mtc0	zero, C0_PAGEMASK
	nop
1:	mtc0	t0, C0_ENTRYHI
	mtc0	t0, C0_INDEX
	nop
	tlbwi
	addi	t0, t0 , 1
	bne	t0, t1, 1b
	nop 
	

	dla	t1, __boot64
	jr	t1
	nop

__boot64:
	j	main
	nop

	.set reorder
	.end	__boot
