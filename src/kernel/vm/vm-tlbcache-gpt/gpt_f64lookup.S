/****************************************************************************
 *      $Id: gpt_f64lookup.S,v 1.1 1998/12/14 02:14:55 gernot Exp $
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

#include "gptdefs.h"
#include <kernel/kernel.h>
#include <regdef.h>
#include <asm.h>
#include <r4kc0.h>
#include <kernel/machine.h>
	
PROC(gpt_lookup)
	/* a0 gpt ptr
	 * a1 vaddr
	 * a2 & size
	 * a3 & paddr
	 */

	daddiu	a0, a0, 15	/* set up P+15 */
	dli	t0, WORDLEN - 4
	daddiu	t1, t0, -GPTROOTSIZE		/* set up r2 */

	.set noreorder
1:	/* 64 bit GPT parsing loop */
	dsrlv	t1, a1, t1
	or	a0, t1
	ld	t0, -15(a0)
	ld	a0, -7(a0)
	xor	a1, t0
	dsrl     t1, a1, t0
	beql    t1, zero, 1b
	dsrl     t1, t0, 6

	/* test if match or page fault */	
	dsrl	a1, t0, 6
	dsllv	t1, t1, a1
	bne	t1, zero, 2f

	/* valid match, return size and paddr */
	andi	t0, 63
	sd	t0, 0(a2)
	sd	a0, 0(a3)
	j	ra
	dli	v0, 0

2:      /* page fault */
	j	ra
	dli	v0, -1

	
END(gpt_lookup)
	.data

PROC(gpt_lookup_ptr)

	/* a0 gpt ptr
	 * a1 vaddr
	 */

	daddiu	a0, a0, 15	/* set up P+15 */
	dli	t0, WORDLEN - 4
	daddiu	t1, t0, -GPTROOTSIZE		/* set up r2 */

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
	nop
	/* valid match, return size and paddr */
	j	ra
	nop

2:      /* page fault */
	j	ra
	dli	v0, 0

	
END(gpt_lookup_ptr)
	
