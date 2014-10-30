/****************************************************************************
 *      $Id: ipc.S,v 1.3 1999/03/08 07:35:22 gernot Exp $
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
#include <regdef.h>
#include <asm.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>	

/****************************************************************************
 * IPC C stubs.
 ****************************************************************************
 * Kernel expects the following
 *
 * Inputs
 * a0 send desc
 * a1 recv desc
 * a2 timeout
 * a3 unused
 * t0 dest tid
 * t1 wfor
 * t2 vsend
 *
 * Outputs
 * v0 cc
 * v1 source thread id
 *
 ****************************************************************************/

/****************************************************************************
 * extern int
 * l4_mips_ipc_call(l4_threadid_t dest, 
 * 		 const void *snd_msg,
 * 		 l4_ipc_reg_msg_t *snd_reg,
 * 		 void *rcv_msg,
 * 		 l4_ipc_reg_msg_t *rcv_reg,
 * 		 l4_timeout_t timeout,
 * 		 l4_msgdope_t *result);
 ****************************************************************************/

#define STACK_C \
	sd	ra, (sp); \
	sd	fp, 8(sp); \
	sd	gp, 16(sp); \
	sd	s7, 24(sp); \
	sd	s6, 32(sp); \
	sd	s5, 40(sp); \
	sd	s4, 48(sp); \
	sd	s3, 56(sp); \
	sd	s2, 64(sp); \
	sd	s1, 72(sp); \
	sd	s0, 80(sp)

#define UNSTACK_C \
	ld	ra, (sp); \
	ld	fp, 8(sp); \
	ld	gp, 16(sp); \
	ld	s7, 24(sp); \
	ld	s6, 32(sp); \
	ld	s5, 40(sp); \
	ld	s4, 48(sp); \
	ld	s3, 56(sp); \
	ld	s2, 64(sp); \
	ld	s1, 72(sp); \
	ld	s0, 80(sp)

#define C_STACK 88	

#define MSG_TO_REG(reg) \
	ld	s0, (reg)	; \
	ld	s1, 8(reg)	; \
	ld	s2, 16(reg)	; \
	ld	s3, 24(reg)	; \
	ld	s4, 32(reg)	; \
	ld	s5, 40(reg)	; \
	ld	s6, 48(reg)	; \
	ld	s7, 56(reg)
	
#define REG_TO_MSG(reg) \
	sd	s0, (reg)	; \
	sd	s1, 8(reg)	; \
	sd	s2, 16(reg)	; \
	sd	s3, 24(reg)	; \
	sd	s4, 32(reg)	; \
	sd	s5, 40(reg)	; \
	sd	s6, 48(reg)	; \
	sd	s7, 56(reg)


#define ARG0  0
#define ARG0_32  4
#define ARG1  8
#define ARG1_32  12
#define ARG2  16
#define ARG2_32  20
#define ARG3  24			
#define ARG3_32  28			
#define ARG4  32
#define ARG4_32  36
#define ARG5  40
#define ARG5_32  44
#define ARG6  48
#define ARG6_32  52
#define ARG7  56
#define ARG7_32  60
#define ARG8  64
#define ARG8_32  68	

/****************************************************************************
 *
 * int
 * l4_mips_ipc_call(l4_threadid_t dest, 
 * 	const void *snd_msg,
 * 	l4_ipc_reg_msg_t *snd_reg,
 * 	void *rcv_msg,
 * 	l4_ipc_reg_msg_t *rcv_reg,
 * 	l4_timeout_t timeout,
 * 	l4_msgdope_t *result);
 *
 ****************************************************************************/	
PROC(l4_mips_ipc_call)
	addiu	sp, sp, -C_STACK
	STACK_C

	MSG_TO_REG(a2)
	
	move	t1, a0
	move	t0, a0
	move	a0, a1
	move	a1, a3
	ld	a2, ARG5+C_STACK(sp)
	 
	.set	noat
	move	AT, zero
	syscall
	.set at
	
	lw	t0, ARG4_32+C_STACK(sp)
	REG_TO_MSG(t0)
	lw	t1, ARG6_32+C_STACK(sp)
	sd	v0, (t1)
	UNSTACK_C
	addiu	sp, sp, C_STACK
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_call)
	
/****************************************************************************
 * extern int 
 * l4_mips_ipc_send(l4_threadid_t dest, 
 * 		 const void *snd_msg,
 * 		 l4_ipc_reg_msg_t *snd_reg,
 * 		 l4_timeout_t timeout,
 * 		 l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_send)
	addiu	sp, sp, -C_STACK
	STACK_C
	
	MSG_TO_REG(a2)

	move	t0, a0
	move	t1, zero
	move	a0, a1
	li	a1, L4_IPC_NIL_DESCRIPTOR
	move	a2, a3

	.set	noat
	move	AT, zero
	syscall
	.set at
	
	lw	t1, ARG4_32+C_STACK(sp)
	sd	v0, (t1)

	UNSTACK_C
	addiu	sp, sp, C_STACK
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_send)


/****************************************************************************
 * extern int l4_mips_ipc_sleep(l4_timeout_t timeout,
 * 	l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_sleep)
	addiu	sp, sp, -C_STACK
	STACK_C
	
	sw	a1, ARG1_32+C_STACK(sp)

	move	a2, a0
	li	a0, L4_IPC_NIL_DESCRIPTOR
	li	a1, L4_IPC_SHORT_MSG
	li	t0, L4_INVALID_ID
	move	t1, t0

	.set	noat
	li	AT, SYSCALL_IPC
	syscall
	.set	at

	lw	t1, ARG1_32+C_STACK(sp)
	sd	v0, (t1)
	UNSTACK_C
	addiu	sp, sp, C_STACK
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_sleep)

/****************************************************************************
 *
 * int 
 * l4_mips_ipc_wait(l4_threadid_t *src,
 * 		 void *rcv_msg,
 * 		 l4_ipc_reg_msg_t *rcv_reg,
 * 		 l4_timeout_t timeout,
 * 		 l4_msgdope_t *result)
 *
 ****************************************************************************/

PROC(l4_mips_ipc_wait)
	addiu	sp, sp, -C_STACK
	STACK_C

	sw	a0, ARG0_32+C_STACK(sp)
	sw	a2, ARG2_32+C_STACK(sp)

	li	a0, L4_IPC_NIL_DESCRIPTOR
	move	a2, a3
	move	t0, zero
	move	t1, zero

	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	lw	a0, ARG0_32+C_STACK(sp)
	sd	v1, (a0)
	lw	a2, ARG2_32+C_STACK(sp)
	REG_TO_MSG(a2)
	lw	t0, ARG4_32+C_STACK(sp)
	sd	v0, (t0)
	UNSTACK_C
	addiu	sp, sp, C_STACK
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_wait)

/****************************************************************************
 *
 * int l4_mips_ipc_reply_and_wait(l4_threadid_t dest, 
 * 			   const void *snd_msg, 
 * 			   l4_ipc_reg_msg_t *snd_reg,
 * 			   l4_threadid_t *src,
 * 			   void *rcv_msg,
 * 			   l4_ipc_reg_msg_t *rcv_reg,
 * 			   l4_timeout_t timeout,
 * 			   l4_msgdope_t *result)
 *
 ****************************************************************************/		
PROC(l4_mips_ipc_reply_and_wait)
	addiu	sp, sp, -C_STACK
	STACK_C

	MSG_TO_REG(a2)
	
	sw	a3, ARG3_32+C_STACK(sp)
	move	t0, a0
	move	t1, zero
	move	a0, a1
	lw	a1, ARG4_32+C_STACK(sp)
	ld	a2, ARG6+C_STACK(sp)

	.set	noat
	li	AT, SYSCALL_IPC
	syscall
	.set	at

	lw	a2, ARG5_32+C_STACK(sp)
	REG_TO_MSG(a2)
	lw	a3, ARG3_32+C_STACK(sp)
	sd	v1, (a3)
	lw	a1, ARG7_32+C_STACK(sp)
	sd	v0, (a1)
	UNSTACK_C
	addiu	sp, sp, C_STACK
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_reply_and_wait)

/****************************************************************************
 *
 * int 
 * l4_mips_ipc_receive(l4_threadid_t src,
 * 		    void *rcv_msg,
 * 		    l4_ipc_reg_msg_t *rcv_reg,
 * 		    l4_timeout_t timeout,
 * 		    l4_msgdope_t *result)
 *
 ****************************************************************************/

PROC(l4_mips_ipc_receive)
	addiu	sp, sp, -C_STACK
	STACK_C

	sw	a2, ARG2_32+C_STACK(sp)
	move	t0, a0
	li	a0, L4_IPC_NIL_DESCRIPTOR
	move	a2, a3
	move	t1, t0

	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	lw	a2, ARG2_32+C_STACK(sp)
	REG_TO_MSG(a2)
	lw	t0, ARG4_32+C_STACK(sp)
	sd	v0, (t0)
	UNSTACK_C
	addiu	sp, sp, C_STACK
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_receive)

/****************************************************************************
 * int 
 * l4_mips_ipc_reply_deceiving_and_wait(l4_threadid_t dest,
 * 				     l4_threadid_t vsend,
 * 				     const void *snd_msg, 
 * 				     l4_ipc_reg_msg_t *snd_reg,
 * 				     l4_threadid_t *src,
 * 				     void *rcv_msg, 
 * 				     l4_ipc_reg_msg_t *rcv_reg,
 * 				     l4_timeout_t timeout, 
 * 				     l4_msgdope_t *result)
 ****************************************************************************/

PROC(l4_mips_ipc_reply_deceiving_and_wait)
	addiu	sp, sp, -C_STACK
	STACK_C

	MSG_TO_REG(a3)
	
	move    t0, a0
	move	t1, zero
	move	t2, a1
	ori	a0, a2, L4_IPC_DECEIT_MASK
	lw	a1, ARG5_32+C_STACK(sp)
	ld	a2, ARG7+C_STACK(sp)
	
	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	lw	a0, ARG6_32+C_STACK(sp)
	REG_TO_MSG(a0)
	lw	a0, ARG4_32+C_STACK(sp)
	sd	v1, (a0)
	lw	a0, ARG8_32+C_STACK(sp)
	sd	v0, (a0)

	UNSTACK_C
	addiu	sp, sp, C_STACK
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_reply_deceiving_and_wait)


/****************************************************************************
 *
 * int 
 * l4_mips_ipc_send_deceiving(l4_threadid_t dest,
 * 			   l4_threadid_t vsend,
 * 			   const void *snd_msg, 
 * 			   l4_ipc_reg_msg_t *snd_reg,
 * 			   l4_timeout_t timeout,
 * 			   l4_msgdope_t *result)
 ****************************************************************************/

PROC(l4_mips_ipc_send_deceiving)
	addiu	sp, sp, -C_STACK
	STACK_C
	
	MSG_TO_REG(a3)

	move	t0, a0
	move	t1, zero
	move	t2, a1
	ori	a0, a2, L4_IPC_DECEIT_MASK
	li	a1, L4_IPC_NIL_DESCRIPTOR
	ld	a2, ARG4+C_STACK(sp)

	.set	noat
	move	AT, zero
	syscall
	.set at
	
	lw	t1, ARG5_32+C_STACK(sp)
	sd	v0, (t1)

	UNSTACK_C
	addiu	sp, sp, C_STACK
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_send_deceiving)
