/****************************************************************************
 * $Id: alphn.S,v 1.2 1999/03/22 06:26:30 gernot Exp $
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
 ****************************************************************************/  
/****************************************************************************
 *
 * Author: Kevin Elphinstone
 * Date: 18 Jan 1996
 * Last Modified: 19 Jan 1996
 *
 ****************************************************************************/

#include <regdef.h>
#include <kernel/p4000i.h>
#include <r4kc0.h>
#include <asm.h>


/* initalise the alphanumeric display to a known state */
	
LEAF(alphn_init)
	.frame sp, 0, ra
	dli	t0, PHYS_TO_CKSEG1(ALPHN_CLEAR)
	sw	zero, (t0)
	li	t1, ~0
	sw	t1, (t0)

	dli	t0,  PHYS_TO_CKSEG1(ALPHN_CURSEN)
	sw	zero, (t0)

	dli	t0, PHYS_TO_CKSEG1(ALPHN_BLANK)
	li	t1, ~0
	sw	t1, (t0)

	jr	ra

END(alphn_init)

/* set alpha display to the 4 chars pointed to by a0 */

LEAF(alphn_set)
	.frame sp, 0, ra
	/* a0 has pointer to 4 chars */
	.set noat
	dli	a1, PHYS_TO_CKSEG1(ALPHN_CHAR_BASE)
	lbu	a2, (a0)
	sll	a2, 24
	sw	a2, 12(a1)
	lbu	a2, 1(a0)
	sll	a2, 24
	sw	a2, 8(a1)
	lbu	a2, 2(a0)
	sll	a2, 24
	sw	a2, 4(a1)
	lbu	a2, 3(a0)
	sll	a2, 24
	sw	a2, (a1)

	jr	ra
	.set at
END(alphn_set)


/* display a long scrolling string on display
   routine doesn't return */
	
LEAF(alphn_string)
	/* expects pointer to null terminated string in a0 */

	move	a1, a0
	dli	t1, PHYS_TO_CKSEG1(ALPHN_CHAR_BASE)

	
	/* print next four characters in array
	   pad with spaces at end */	
do:	
	lbu	t0, (a1)
	beq	t0, zero, 1f
	sll	t0, 24
	sw	t0, 12(t1)
	lbu	t0, 1(a1)
	beq	t0, zero, 2f
	sll	t0, 24
	sw	t0, 8(t1)
	lbu	t0, 2(a1)
	beq	t0, zero, 3f
	sll	t0, 24
	sw	t0, 4(t1)
	lbu	t0, 3(a1)
	beq	t0, zero, 4f
	sll	t0, 24
	sw	t0, (t1)
	b	5f
1:
	move	a1, a0
	dsubu	a1, 1
	li	t0, ' ' << 24
	sw	t0, 12(t1)
2:
	li	t0, ' ' << 24
	sw	t0, 8(t1)
3:
	li	t0, ' ' << 24
	sw	t0, 4(t1)
4:
	li	t0, ' ' << 24
	sw	t0, 0(t1)
5:
	/* delay */
	
	li	t0, 6000000
1:	subu    t0, 1
	bne	t0, zero, 1b	

	daddu	a1,1
	j	do
	
END(alphn_string)



