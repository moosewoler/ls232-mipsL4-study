/****************************************************************************
 *      $Id: panic.S,v 1.2 1999/10/01 05:08:34 alanau Exp $
 *      Copyright (C) 1997, 1998 Kevin Elphinstone, Univeristy of New South
 *      Wales.
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
#include <asm.h>
#include <regdef.h>
#include <r4kc0.h>
#include <kernel/machine.h>
#include <kernel/kernel.h>
	
PROC(panic)
	
	/* switch off interrupts, trashes AT */
	.set noat
	.set noreorder
	mfc0	AT, C0_STATUS
	srl	AT, 1
	sll	AT, 1
	mtc0	AT, C0_STATUS
	nop

	.set reorder

	dla	AT, regset
	sd	ra, 16(AT)
	sd	s8, 24(AT)
	sd	gp, 32(AT)
	sd	t9, 40(AT)
	sd	t8, 48(AT)
	sd	s7, 56(AT)
	sd	s6, 64(AT)
	sd	s5, 72(AT)
	sd	s4, 80(AT)
	sd	s3, 88(AT)
	sd	s2, 96(AT)
	sd	s1, 104(AT)
	sd	s0, 112(AT)
	sd	t3, 120(AT)
	sd	t2, 128(AT)
	sd	t1, 136(AT)
	sd	t0, 144(AT)
	sd	a7, 152(AT)
	sd	a6, 160(AT)
	sd	a5, 168(AT)
	sd	a4, 176(AT)
	sd	a3, 184(AT)
	sd	a2, 192(AT)
	sd	a1, 200(AT)
	sd	a0, 208(AT)
	sd	v1, 216(AT)
	sd	v0, 224(AT)
	sd	zero, 232(AT)
	sd	sp, 240(AT)
	sd	k0, 248(AT)
	sd	k1, 256(AT)
	mfhi	v0
	sd	v0, (AT)
	mflo	v0
	sd	v0, 8(AT)
	
	.set at
	/* a0 contains pointer to string */
	move	s1, a0

	
	/* set a valid stack to run the debugger in */
	/* use first (empty) tcb block */

	dli	sp, DEBUGGER_STACK

	/* init the serial port, will trash s0 */
	jal	reset_serial_io
	move	a0, s1
	j	debug
END(panic)

PROC(panic_reg_stack)
	
	/* interrupts already off */

	/* a0 contains pointer to string */
	move	s1, a0
	move	s2, ra

	/* copy reg set from stack to where debugger expects them */
	
	dla	k0, regset
	
	ld	k1, (sp)
	sd	k1, (k0)
	
	ld	k1, 8(sp)
	sd	k1, 8(k0)

	ld	k1, 16(sp)
	sd	k1, 16(k0)
	
	ld	k1, 24(sp)
	sd	k1, 24(k0)
	
	ld	k1, 32(sp)
	sd	k1, 32(k0)
	
	ld	k1, 40(sp)
	sd	k1, 40(k0)

	ld	k1, 48(sp)
	sd	k1, 48(k0)

	ld	k1, 56(sp)
	sd	k1, 56(k0)
	
	ld	k1, 64(sp)
	sd	k1, 64(k0)
	
	ld	k1, 72(sp)
	sd	k1, 72(k0)
	
	ld	k1, 80(sp)
	sd	k1, 80(k0)
	
	ld	k1, 88(sp)
	sd	k1, 88(k0)
	
	ld	k1, 96(sp)
	sd	k1, 96(k0)
	
	ld	k1, 104(sp)
	sd	k1, 104(k0)
	
	ld	k1, 112(sp)
	sd	k1, 112(k0)
	
	ld	k1, 120(sp)
	sd	k1, 120(k0)
	
	ld	k1, 128(sp)
	sd	k1, 128(k0)
	
	ld	k1, 136(sp)
	sd	k1, 136(k0)
	
	ld	k1, 144(sp)
	sd	k1, 144(k0)
	
	ld	k1, 152(sp)
	sd	k1, 152(k0)
	
	ld	k1, 160(sp)
	sd	k1, 160(k0)
	
	ld	k1, 168(sp)
	sd	k1, 168(k0)
	
	ld	k1, 176(sp)
	sd	k1, 176(k0)
	
	ld	k1, 184(sp)
	sd	k1, 184(k0)
	
	ld	k1, 192(sp)
	sd	k1, 192(k0)
	
	ld	k1, 200(sp)
	sd	k1, 200(k0)
	
	ld	k1, 208(sp)
	sd	k1, 208(k0)
	
	ld	k1, 216(sp)
	sd	k1, 216(k0)

	ld	k1, 240(sp) /* original stack pointer */
	sd	k1, 224(k0)
	
	/* set a valid stack to run the debugger in */
	/* use first (empty) tcb block */
	.set	noreorder
	dli	k0, DEBUGGER_STACK
	mfc0	k1, C0_STATUS
	sw	k1, -8(k0)
	sd	sp, -16(k0)
	daddiu	sp, k0, -16
	.set	reorder
	/* init the serial port, will trash s0 */
	jal	reset_serial_io
	move	a0, s1
	jal	debug
	
	dli	k0, DEBUGGER_STACK
	ld	sp, -16(k0) /* get old sp */
	jr	s2
END(panic_reg_stack)

PROC(get_bva)
	.set noreorder
	dmfc0	t0, C0_BADVADDR
	nop
	sd	t0,(a0)
	jr	ra
	nop
	.set reorder
END(get_bva)


PROC(get_epc)
	.set noreorder
	dmfc0	t0, C0_EPC
	nop
	sd	t0,(a0)
	jr	ra
	nop
	.set reorder
END(get_epc)

PROC(get_ehi)
	.set noreorder
	dmfc0	t0, C0_ENTRYHI
	nop
	sd	t0,(a0)
	jr	ra
	nop
	.set reorder
END(get_ehi)

PROC(get_prid)
	.set noreorder
	mfc0	t0, C0_PRID
	nop
	sd	t0,(a0)
	jr	ra
	nop
	.set reorder
END(get_prid)

PROC(get_xc)
	.set noreorder
	dmfc0	t0, C0_XCONTEXT
	nop
	sd	t0,(a0)
	jr	ra
	nop
	.set reorder
END(get_xc)

PROC(get_st)
	.set noreorder
	mfc0	t0, C0_STATUS
	nop
	sd	t0,(a0)
	jr	ra
	nop
	.set reorder
END(get_st)

PROC(get_cs)
	.set noreorder
	mfc0	t0, C0_CAUSE
	nop
	sd	t0,(a0)
	jr	ra
	nop
	.set reorder
END(get_cs)

PROC(get_cnt)
	.set noreorder
	mfc0	t0, C0_COUNT
	nop
	sd	t0,(a0)
	jr	ra
	nop
	.set reorder
END(get_cnt)

PROC(get_cmp)
	.set noreorder
	mfc0	t0, C0_COMPARE
	nop
	sd	t0,(a0)
	jr	ra
	nop
	.set reorder
END(get_cmp)



PROC(get_tlb)
	.set noreorder
	dmfc0	t8, C0_ENTRYHI
	li	t0, 47

1:	mtc0	t0, C0_INDEX
	nop
	tlbr
	nop
	dmfc0	t1, C0_ENTRYHI
	dmfc0	t2, C0_ENTRYLO0
	sd	t1, (a0)
	dmfc0	t1, C0_ENTRYLO1
	sw	t2, 12(a0)
	sw	t1, 8(a0)
	daddiu	a0, a0, 16
	daddiu	t0, t0, -1
	bgez	t0, 1b
	nop
	dmtc0	t8, C0_ENTRYHI
	jr	ra
	nop
	.set reorder
END(get_tlb)

	.data
break_msg:
	.asciiz	"break"

PROC(dbg)
	/* switch off interrupts off, stack status, and old sp */
	.set noat
	.set noreorder

	dli	k0, DEBUGGER_STACK
	mfc0	k1, C0_STATUS
	sw	k1, -8(k0)
	srl	k1, 1
	sll	k1, 1
	mtc0	k1, C0_STATUS
	sd	sp, -16(k0)
	daddiu	sp, k0, -16

	/* now save register set */

	dla	k0, regset
	sd	ra, 16(k0)
	sd	s8, 24(k0)
	sd	gp, 32(k0)
	sd	t9, 40(k0)
	sd	t8, 48(k0)
	sd	s7, 56(k0)
	sd	s6, 64(k0)
	sd	s5, 72(k0)
	sd	s4, 80(k0)
	sd	s3, 88(k0)
	sd	s2, 96(k0)
	sd	s1, 104(k0)
	sd	s0, 112(k0)
	sd	t3, 120(k0)
	sd	t2, 128(k0)
	sd	t1, 136(k0)
	sd	t0, 144(k0)
	sd	a7, 152(k0)
	sd	a6, 160(k0)
	sd	a5, 168(k0)
	sd	a4, 176(k0)
	sd	a3, 184(k0)
	sd	a2, 192(k0)
	sd	a1, 200(k0)
	sd	a0, 208(k0)
	sd	v1, 216(k0)
	sd	v0, 224(k0)
	sd	AT, 232(k0)	
	ld	v0, 0(sp) /* original stack pointer */
	sd	v0, 240(k0)
	dli	k1, 0x12345678
	sd	k1, 248(k0) /* k0 */
	sd	k1, 256(k0) /* k1 */

	mfhi	v0
	sd	v0, (k0)
	mflo	v0
	sd	v0, 8(k0)
	
	
	/* reset serial port in case it is not started */
	
	jal	reset_serial_io
	nop
	/* call debugger */
	
	dla	a0, break_msg
	jal	debug
	nop
	
	/* now restore registers for restart */

	dla	k0, regset
	ld	t0, (k0)
	mthi	t0
	ld	t0, 8(k0)
	mtlo	t0
	ld	ra, 16(k0)
	ld	s8, 24(k0)
	ld	gp, 32(k0)
	ld	t9, 40(k0)
	ld	t8, 48(k0)
	ld	s7, 56(k0)
	ld	s6, 64(k0)
	ld	s5, 72(k0)
	ld	s4, 80(k0)
	ld	s3, 88(k0)
	ld	s2, 96(k0)
	ld	s1, 104(k0)
	ld	s0, 112(k0)
	ld	t3, 120(k0)
	ld	t2, 128(k0)
	ld	t1, 136(k0)
	ld	t0, 144(k0)
	ld	a7, 152(k0)
	ld	a6, 160(k0)
	ld	a5, 168(k0)
	ld	a4, 176(k0)
	ld	a3, 184(k0)
	ld	a2, 192(k0)
	ld	a1, 200(k0)
	ld	a0, 208(k0)
	ld	v1, 216(k0)
	ld	v0, 224(k0)
	ld	AT, 232(k0)
	
	dli	k0, DEBUGGER_STACK
	lw	k1, -8(k0)
	ld	sp, -16(k0) /* get old sp */

	/* restore old co-pro status */
	mtc0	k1, C0_STATUS
	nop
	jr	ra
	nop
	.set	at
	.set reorder
END(dbg)

PROC(rbt_comm)
	dla	t0, 0xffffffffbfc00000
	jr	t0
END(rbt_comm)	


