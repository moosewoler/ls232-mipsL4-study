/****************************************************************************
 * $Id: new_task.S,v 1.1 1998/12/28 12:23:18 gernot Exp $
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
#include <asm.h>
#include <regdef.h>
#include <l4/ipc.h>

	
PROC(main)
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_NIL_DESCRIPTOR
	dli	a2, 0
	dli	a4, 0x10020000000c0001
	.set noat
	move	AT, zero
	syscall
1:	nop
	b	1b
END(main)
