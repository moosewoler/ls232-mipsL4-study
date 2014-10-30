/****************************************************************************
 * $Id: ipc_test_client_asm.S,v 1.2 1999/10/23 07:11:36 gernot Exp $
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
#include <l4/syscalls.h>
#include <l4/ipc.h>
	
#ifdef INDY
#include <kernel/indy.h>
#endif

PROC(ping_thread)
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_SHORT_MSG
	dli	a2, 0
	dli	a4, 0x10020000000a0001
	move	a5, zero
	.set noat
	move	AT, zero
	syscall
1:	nop
	b	1b
END(ping_thread)


PROC(client_loop)
	/* preserve c */
	daddiu	sp, sp, -96
	sd	ra, (sp)
	sd	fp, 8(sp)
	sd	gp, 16(sp)
	sd	s7, 24(sp)
	sd	s6, 32(sp)
	sd	s5, 40(sp)
	sd	s4, 48(sp)
	sd	s3, 56(sp)
	sd	s2, 64(sp)
	sd	s1, 72(sp)
	sd	s0, 80(sp)

	li	t0, 1000
	sw	t0, 88(sp)
	
	.set	noreorder
	.set	noat
	
	dli	a4, 0x1002000000080001
	move	a5, a4
	move	a0, zero
	move	a1, zero
	move	a2, zero
	move	AT, zero

1:	syscall
	lw	t0, 88(sp)
	dli	a4, 0x1002000000080001
	move	a5, zero
	move	a0, zero
	move	a1, zero
	move	a2, zero
	move	AT, zero
	addiu	t0, t0, -1
	bne	t0, zero, 1b
	sw	t0, 88(sp)

	.set	at
	.set	reorder
	
	ld	ra, (sp)
	ld	fp, 8(sp)
	ld	gp, 16(sp)
	ld	s7, 24(sp)
	ld	s6, 32(sp)
	ld	s5, 40(sp)
	ld	s4, 48(sp)
	ld	s3, 56(sp)
	ld	s2, 64(sp)
	ld	s1, 72(sp)
	ld	s0, 80(sp)
	daddiu	sp, sp, 96
	jr	ra
END(client_loop)

PROC(local_client_loop)
	/* preserve c */
	daddiu	sp, sp, -96
	sd	ra, (sp)
	sd	fp, 8(sp)
	sd	gp, 16(sp)
	sd	s7, 24(sp)
	sd	s6, 32(sp)
	sd	s5, 40(sp)
	sd	s4, 48(sp)
	sd	s3, 56(sp)
	sd	s2, 64(sp)
	sd	s1, 72(sp)
	sd	s0, 80(sp)

	li	t0, 1000
	sw	t0, 88(sp)
	
	.set	noreorder
	.set	noat
	
	dli	a4, 0x10020000000a2001
	move	a5, a4
	move	a0, zero
	move	a1, zero
	move	a2, zero
	move	AT, zero

1:	syscall
	lw	t0, 88(sp)
	dli	a4, 0x10020000000a2001
	move	a5, zero
	move	a0, zero
	move	a1, zero
	move	a2, zero
	move	AT, zero
	addiu	t0, t0, -1
	bne	t0, zero, 1b
	sw	t0, 88(sp)

	.set	at
	.set	reorder
	
	ld	ra, (sp)
	ld	fp, 8(sp)
	ld	gp, 16(sp)
	ld	s7, 24(sp)
	ld	s6, 32(sp)
	ld	s5, 40(sp)
	ld	s4, 48(sp)
	ld	s3, 56(sp)
	ld	s2, 64(sp)
	ld	s1, 72(sp)
	ld	s0, 80(sp)
	daddiu	sp, sp, 96
	jr	ra
END(local_client_loop)

PROC(local_server_loop)
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
END(local_server_loop)

PROC(ex_reg_loop)
	/* preserve c */
	daddiu	sp, sp, -96
	sd	ra, (sp)
	sd	fp, 8(sp)
	sd	gp, 16(sp)
	sd	s7, 24(sp)
	sd	s6, 32(sp)
	sd	s5, 40(sp)
	sd	s4, 48(sp)
	sd	s3, 56(sp)
	sd	s2, 64(sp)
	sd	s1, 72(sp)
	sd	s0, 80(sp)

	li	t0, 1000000
	sw	t0, 88(sp)
	
	.set	noreorder
	.set	noat
	
	dli	a0, 1
	dla	a1, thread_1
	dla	a2, thread_1_stack
	daddiu	a2, a2, 127*8
	dli	a3, -1
	dli	a4, -1
	dli	AT,  SYSCALL_LTHREAD_EX_REG

1:	syscall
	lw	t0, 88(sp)
	dli	a0, 1
	dla	a1, thread_1
	dla	a2, thread_1_stack
	daddiu	a2, a2, 127*8
	dli	a3, -1
	dli	a4, -1
	dli	AT,  SYSCALL_LTHREAD_EX_REG
	addiu	t0, t0, -1
	bne	t0, zero, 1b
	sw	t0, 88(sp)

	.set	at
	.set	reorder
	
	ld	ra, (sp)
	ld	fp, 8(sp)
	ld	gp, 16(sp)
	ld	s7, 24(sp)
	ld	s6, 32(sp)
	ld	s5, 40(sp)
	ld	s4, 48(sp)
	ld	s3, 56(sp)
	ld	s2, 64(sp)
	ld	s1, 72(sp)
	ld	s0, 80(sp)
	daddiu	sp, sp, 96
	jr	ra
END(ex_reg_loop)

PROC(id_loop)
	/* preserve c */
	daddiu	sp, sp, -96
	sd	ra, (sp)
	sd	fp, 8(sp)
	sd	gp, 16(sp)
	sd	s7, 24(sp)
	sd	s6, 32(sp)
	sd	s5, 40(sp)
	sd	s4, 48(sp)
	sd	s3, 56(sp)
	sd	s2, 64(sp)
	sd	s1, 72(sp)
	sd	s0, 80(sp)

	li	t0, 1000000
	sw	t0, 88(sp)
	
	.set	noreorder
	.set	noat
	move	a0, zero	
	dli	AT,  SYSCALL_ID_NEAREST

1:	syscall
	lw	t0, 88(sp)
	dli	AT,  SYSCALL_ID_NEAREST
	move	a0, zero
	addiu	t0, t0, -1
	bne	t0, zero, 1b
	sw	t0, 88(sp)

	.set	at
	.set	reorder
	
	ld	ra, (sp)
	ld	fp, 8(sp)
	ld	gp, 16(sp)
	ld	s7, 24(sp)
	ld	s6, 32(sp)
	ld	s5, 40(sp)
	ld	s4, 48(sp)
	ld	s3, 56(sp)
	ld	s2, 64(sp)
	ld	s1, 72(sp)
	ld	s0, 80(sp)
	daddiu	sp, sp, 96
	jr	ra
END(id_loop)

#ifdef INDY
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
	
#endif
