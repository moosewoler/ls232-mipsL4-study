/****************************************************************************
 * $Id: l_iserial.S,v 1.1 1999/03/22 04:58:34 gernot Exp $
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
#include <kernel/indy.h>
#include <r4kc0.h>

PROC(wrreg)
	move	t8, ra
	dli	t0, HPC3_SERIAL1_CMD | 3
	dli	t1, HPC3_SERIAL1_DATA | 3
	dli	t2, HPC3_INTSTAT_ADDR
	jal	dl
	sb	a0, (t0)
	sync
	lw	zero, (t2)
	jal	dl
	sb	a1, (t0)
	sync
	lw	zero, (t2)
	jr	t8
END(wrreg)

PROC(rdreg)
	move	t8, ra
	dli	t0, HPC3_SERIAL1_CMD | 3
	dli	t1, HPC3_SERIAL1_DATA | 3
	dli	t2, HPC3_INTSTAT_ADDR
	jal	dl
	sb	a0, (t0)
	sync
	lw	zero, (t2)
	jal	dl
	lbu	v0, (t0)
	jr	t8
END(rdreg)

PROC(rdzero)
	move	t8, ra
	dli	t0, HPC3_SERIAL1_CMD | 3
	jal	dl
	lbu	v0, (t0)
	jr	t8
END(rdzero)

PROC(dl)
	dli	t9, 66
	
1:	daddiu	t9, t9, -1
	bne	t9, zero, 1b

	
	jr	ra
END(dl)
