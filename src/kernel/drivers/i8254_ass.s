/****************************************************************************
 * $Id: i8254_ass.s,v 1.3 1998/01/22 05:46:59 kevine Exp $
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
 ****************************************************************************/ #include <asm.h>
#include <regdef.h>
#include <kernel/indy.h>
#include <r4kc0.h>


/*
 * Definitions for 8254 programmable interval timer
 *
 * NOTE: counter2 is clocked at MASTER_FREQ (defined below), the
 * output of counter2 is the clock for both counter0 and counter1.
 * For the IP17/IP20/IP22, counter0 output is tied to interrupt 2 for the
 * scheduling clock, and interrupt 3 for the 'fast' clock.
 * For all other CPUs, counter0 output is tied to Interrupt 2 to act 
 * as the scheduling clock and the output of counter1 is tied to 
 * Interrupt 4 to act as the profiling clock.
 */
/*
 * control word definitions
 */

#define PTCW_SC(x)      ((x)<<6)        /* select counter x */
#define PTCW_RBCMD      (3<<6)          /* read-back command */
#define PTCW_CLCMD      (0<<4)          /* counter latch command */
#define PTCW_LSB        (1<<4)          /* r/w least signif. byte only */
#define PTCW_MSB        (2<<4)          /* r/w most signif. byte only */
#define PTCW_16B        (3<<4)          /* r/w 16 bits, lsb then msb */
#define PTCW_MODE(x)    ((x)<<1)        /* set mode to x */
#define PTCW_BCD        0x1             /* operate in BCD mode */

/*
 * Mode definitions
 */
#define MODE_ITC        0               /* interrupt on terminal count */
#define MODE_HROS       1               /* hw retriggerable one-shot */
#define MODE_RG         2               /* rate generator */
#define MODE_SQW        3               /* square wave generator */
#define MODE_STS        4               /* software triggered strobe */
#define MODE_HTS        5               /* hardware triggered strobe */

PROC(sync_wait)
	sync
	dli	t9, 3
1:	daddiu	t9, t9, -1
	bne	t9, zero, 1b
	jr	ra
END(sync_wait)

PROC(timestamp)
	move	t8, ra
	dli	t0, PT_CLOCK_ADDR
	li	t1, PTCW_SC(2) | PTCW_CLCMD
	sb	t1, PT_CONTROL(t0)
	sync
	nop
	li	t1, PTCW_SC(1) | PTCW_CLCMD
	sb	t1, PT_CONTROL(t0)
	sync
	nop
	move	v0, zero
	lbu	a2, PT_COUNTER1(t0)
	sll	a2, 16
	or	v0, v0, a2
	nop
	lbu	a3, PT_COUNTER1(t0)
	sll	a3, 24
	or	v0, v0, a3
	nop
	lbu	a0, PT_COUNTER2(t0)
	or	v0, v0, a0
	nop
	nop
	lbu	a1, PT_COUNTER2(t0)
	sll	a1, 8
	or	v0, v0, a1
	jr	t8
END(timestamp)
