/****************************************************************************
 * $Id: serial_init.S,v 1.1 1998/12/14 02:13:49 gernot Exp $
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
#include <kernel/p4000i.h>
#include <r4kc0.h>

PROC(reset_serial_io)
#if 1	
	li	t0, -1
	sw	t0, PHYS_TO_KSEG1(SIO_RESET) /* enable serial port */
	/* assume called from panic and can trash s0 */
	move	s0, ra
	jal	wbflush
	jal	devinit
	jr	s0
#else
	jr	ra
#endif	
END(reset_serial_io)
	
