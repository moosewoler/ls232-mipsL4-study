/****************************************************************************
 * $Id: dlr1414.S,v 1.1 1998/12/14 02:13:56 gernot Exp $
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
#include <regdef.h>
#include <asm.h>
#include <kernel/machine.h>
#include <r4kc0.h>
#include <kernel/u4600.h>

	.globl	alphn_init
	.ent	alphn_init
alphn_init:
	jr	ra
	.end	alphn_init

PROC(alphn_set)
	.frame sp, 0, ra
	/* a0 has pointer to 4 chars */
	.set noat
	dli	a1, PHYS_TO_CKSEG1(LED_BASE)
	lbu	a2, (a0)
	sb	a2, 3(a1)
	lbu	a2, 1(a0)
	sb	a2, 2(a1)
	lbu	a2, 2(a0)
	sb	a2, 1(a1)
	lbu	a2, 3(a0)
	sb	a2, (a1)

	jr	ra
	.set at
END(alphn_set)
