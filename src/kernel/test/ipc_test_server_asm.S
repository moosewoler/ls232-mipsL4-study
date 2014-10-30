/****************************************************************************
 * $Id: ipc_test_server_asm.S,v 1.1 1998/12/28 12:23:14 gernot Exp $
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
#include <l4/ipc.h>

PROC(server_loop)
	.set	noreorder
	.set	noat
	move	a4, a0
	move	a5, zero
	
1:	move	a0, zero
	move	a1, zero
	move	a2, zero
	move	AT, zero
	syscall
	move	a4, v1
	b	1b
	move	a5, zero
END(server_loop)

PROC(mem_server_loop)
	.set	noreorder
	.set	noat
	dli	a0, L4_IPC_NIL_DESCRIPTOR
	dli	a1, L4_IPC_SHORT_MSG
	dli	a2, L4_IPC_NEVER
	dli	a5, 0
	dli	AT, 0
1:	syscall
	dli	a0, L4_IPC_SHORT_FPAGE
	dli	a1, L4_IPC_SHORT_MSG
	dli	a2, L4_IPC_NEVER
	move	a4, v1
	dli	a5, 0
	dsrl	s1, s0, 12
	dsll	s1, s1, 12
	ori	s1, s1, ((L4_LOG2_PAGESIZE << 2) | 2)
	move	s3, zero
	b	1b
	dli	AT, 0
END(mem_server_loop)
