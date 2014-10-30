/****************************************************************************
 *      $Id: ipc.S,v 1.9 1999/02/10 04:56:55 gernot Exp $
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
#include "macros.h"

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
 * a4 dest tid
 * a5 wfor
 * a6 vsend
 *
 * Outputs
 * v0 cc
 * v1 source thread id
 *
 ****************************************************************************/

/****************************************************************************
 *
 * extern int
 * l4_mips_ipc_reply_deceiving_and_wait(l4_threadid_t dest,
 * 				     l4_threadid_t vsend,
 * 				     const void *snd_msg,
 * 				     l4_ipc_reg_msg_t *snd_reg,
 * 				     l4_threadid_t *src,
 * 				     void *rcv_msg,
 * 				     l4_ipc_reg_msg_t *rcv_reg,
 * 				     l4_timeout_t timeout,
 * 				     l4_msgdope_t *result);
 *
 ****************************************************************************/

PROC(l4_mips_ipc_reply_deceiving_and_wait)
	daddiu	sp, sp, -104
	stack_c
	sd	a6, 88(sp) /* *recv_reg */
	sd	a4, 96(sp) /* *src */
	msg_to_reg(a3) /* s0 - s7 */


	move	a6, a1 /* vsend */
	move	a1, a5 /* rdesc */
	move	a5, zero /* wfor */
	move	a4, a0 /* dest */
	ori	a0, a2, L4_IPC_DECEIT_MASK  /* send desc */
	move	a2, a7 /* timeout */

	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	ld	a0, 96(sp)
	sd	v1, (a0)
	ld	a1, 88(sp)
	reg_to_msg(a1)
	ld	a0, 104(sp)
	sd	v0, (a0)
	unstack_c
	daddiu	sp, sp, 104
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_reply_deceiving_and_wait)


/****************************************************************************
 * extern int
 * l4_mips_ipc_send_deceiving(l4_threadid_t dest,
 * 			   l4_threadid_t vsend,
 * 			   const void *snd_msg,
 * 			   l4_ipc_reg_msg_t *snd_reg,
 * 			   l4_timeout_t timeout,
 * 			   l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_send_deceiving)
	daddiu	sp,-96
	stack_c
	msg_to_reg(a3)
	sd	a5, 88(sp)
	move	a6, a1
	dli	a1,  L4_IPC_NIL_DESCRIPTOR
	move	a5, a0
	ori	a0, a2, L4_IPC_DECEIT_MASK
	move	a2, a4
	move	a4, a5
	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at
 	ld	a0, 88(sp)
	sd	v0, (a0)
	unstack_c
	daddiu	sp, 96
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_send_deceiving)


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

PROC(l4_mips_ipc_call)
	daddiu	sp,-104
	stack_c
	sd	a4, 88(sp)
	sd	a6, 96(sp)
	msg_to_reg(a2)

	move	a6, a0
	move	a0, a1
	move	a1, a3
	move	a2, a5
	move	a4, a6
	move	a5, a6
/*	move	a6, zero */


	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	ld	a0, 88(sp)
	reg_to_msg(a0)
	ld	a1, 96(sp)
	sd	v0, (a1)
	unstack_c
	daddiu	sp, 104
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_call)

/****************************************************************************
 * extern int
 * l4_mips_ipc_reply_and_wait(l4_threadid_t dest,
 * 			   const void *snd_msg,
 * 			   l4_ipc_reg_msg_t *snd_reg,
 * 			   l4_threadid_t *src,
 * 			   void *rcv_msg,
 * 			   l4_ipc_reg_msg_t *rcv_reg,
 * 			   l4_timeout_t timeout,
 * 			   l4_msgdope_t *result);
 ****************************************************************************
 *
 */

PROC(l4_mips_ipc_reply_and_wait)
	daddiu	sp,-112
	stack_c
	sd	a3, 88(sp)
	sd	a5, 96(sp)
	sd	a7, 104(sp)
	msg_to_reg(a2)


	move	t0, a0
	move	a0, a1
	move	a1, a4
	move	a4, t0
	move	a2, a6
	move	a5, zero
/*	move	a6, zero */

	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	ld	a0, 88(sp)
	sd	v1, (a0)
	ld	a1, 96(sp)
	reg_to_msg(a1)
	ld	a0, 104(sp)
	sd	v0, (a0)
	unstack_c
	daddiu	sp, 112
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_reply_and_wait)


/****************************************************************************
 * extern int
 * l4_mips_ipc_send(l4_threadid_t dest,
 * 		 const void *snd_msg,
 * 		 l4_ipc_reg_msg_t *snd_reg,
 * 		 l4_timeout_t timeout,
 * 		 l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_send)
	daddiu	sp,-96
	stack_c
	msg_to_reg(a2)
	sd	a4,88(sp)
	move	a4, a0
	move	a0, a1
	dli	a1,  L4_IPC_NIL_DESCRIPTOR
	move	a2, a3
	.set	noat
	dli	AT, SYSCALL_IPC
/*	dli	a6, 0 */
/* 	dli	a5, 0 */
	syscall
	.set	at
	ld	a0, 88(sp)
	sd	v0, (a0)
	unstack_c
	daddiu	sp, 96
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_send)

/****************************************************************************
 * l4_mips_ipc_receive(l4_threadid_t src,
 * 		    void *rcv_msg,
 * 		    l4_ipc_reg_msg_t *rcv_reg,
 * 		    l4_timeout_t timeout,
 * 		    l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_receive)
	daddiu	sp,-104
	stack_c
	sd	a2, 88(sp)
	sd	a4, 96(sp)
	move	a5, a0
	dli	a0, L4_IPC_NIL_DESCRIPTOR
	move	a2, a3
/*	move	a4, a5 */
/*	dli	a6, 0  */

	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	ld	a0, 88(sp)
	reg_to_msg(a0)
	unstack_c
	ld	a0, 96(sp)
	sd	v0, (a0)
	daddiu	sp, 104
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_receive)


/****************************************************************************
 * extern int
 * l4_mips_ipc_wait(l4_threadid_t *src,
 * 		 void *rcv_msg,
 * 		 l4_ipc_reg_msg_t *rcv_reg,
 * 		 l4_timeout_t timeout,
 * 		 l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_wait)
	daddiu	sp,-112
	stack_c
	sd	a0, 88(sp)
	sd	a2, 96(sp)
	sd	a4, 104(sp)

	dli	a0, L4_IPC_NIL_DESCRIPTOR
	move	a2, a3
/*	move	a4, zero */
	move	a5, zero
/*	move	a6, zero */
	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	ld	a0, 88(sp)
	sd	v1, (a0)
	ld	a0, 96(sp)
	reg_to_msg(a0)
	unstack_c
	ld	a0, 104(sp)
	sd	v0, (a0)
	daddiu	sp, 112
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_wait)

/****************************************************************************
 * extern int l4_ipc_sleep(l4_timeout_t timeout,
 * 	l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_sleep)
	daddiu	sp,-96
	stack_c
	move	a2, a0
	dli	a0, L4_IPC_NIL_DESCRIPTOR
	sd	a1, 88(sp)
	dli	a1, L4_IPC_SHORT_MSG
	dli	a5, L4_INVALID_ID
/* 	move	a4, a5 */
/*	dli	a6, 0  */
	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	unstack_c
	daddiu	sp, 96
	jr	ra
END(l4_mips_ipc_sleep)


/****************************************************************************
 * IPV bindings for chiefs
 ****************************************************************************/


/****************************************************************************
 * extern int
 * l4_mips_ipc_chief_wait(l4_threadid_t *src,
 *			  l4_threadid_t *real_dst,
 *			  void *rcv_msg,
 *			  l4_ipc_reg_msg_t *rcv_reg,
 *			  l4_timeout_t timeout,
 *			  l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_chief_wait)
	daddiu	sp,-120
	stack_c
	sd	a0, 88(sp)
	sd	a1, 96(sp)
	sd	a3, 104(sp)
	sd	a5, 112(sp)

	dli	a0, L4_IPC_NIL_DESCRIPTOR
	move	a1, a2
	move	a2, a4
/*	move	a4, zero */
	move	a5, zero
/*	move	a6, zero */

	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	ld	a0, 88(sp)
	sd	v1, (a0)
	ld	a1, 96(sp)
	sd	a4, (a1)
	ld	a0, 104(sp)
	reg_to_msg(a0)
	unstack_c
	ld	a0, 112(sp)
	sd	v0, (a0)
	daddiu	sp, 120
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_chief_wait)


/****************************************************************************
 * l4_mips_ipc_chief_receive(l4_threadid_t src,
 *			     l4_threadid_t *real_dst,
 *			     void *rcv_msg,
 *			     l4_ipc_reg_msg_t *rcv_reg,
 *			     l4_timeout_t timeout,
 *			     l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_chief_receive)
	daddiu	sp,-112
	stack_c
	sd	a1, 88(sp)
	sd	a3, 96(sp)
	sd	a5, 104(sp)
	move	a1, a2
	move	a2, a4
	move	a5, a0
	dli	a0, L4_IPC_NIL_DESCRIPTOR
/*	move	a5, a4 */
/*	dli	a6, 0  */

	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	ld	a0, 96(sp)
	reg_to_msg(a0)
	ld	a1, 88(sp)
	sd	a4, (a1)
	unstack_c
	ld	a0, 104(sp)
	sd	v0, (a0)
	daddiu	sp, 112
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_chief_receive)


/****************************************************************************
 * extern int
 * l4_mips_ipc_chief_call(l4_threadid_t dest,
 *		          l4_threadid_t vsend,
 *			  const void *snd_msg,
 *			  l4_ipc_reg_msg_t *snd_reg,
 *		          l4_threadid_t *real_dst,
 *			  void *rcv_msg,
 *			  l4_ipc_reg_msg_t *rcv_reg,
 *			  l4_timeout_t timeout,
 *			  l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_chief_call)
	daddiu	sp,-104
	stack_c
	sd	a4, 88(sp)
	sd	a6, 96(sp)
	/* Note: *result is already on stack (at offset 104) */
	msg_to_reg(a3)

	move	a4, a0
	ori	a0, a2, L4_IPC_DECEIT_MASK  /* send desc */
	move	a2, a7
	move	a6, a1
	move	a1, a5
	move	a5, a4
/*	move	a6, zero */

	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	ld	a1, 88(sp)
	sd	a4, (a1)
	ld	a0, 96(sp)
	reg_to_msg(a0)
	ld	a1, 104(sp)
	sd	v0, (a1)
	unstack_c
	daddiu	sp, 104
	andi	v0, v0,  L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_chief_call)


/****************************************************************************
 * extern int
 * l4_mips_ipc_chief_reply_and_wait(l4_threadid_t dest,
 *				    l4_threadid_t vsend,
 *				    const void *snd_msg,
 *				    l4_ipc_reg_msg_t *snd_reg,
 *				    l4_threadid_t *src,
 *				    l4_threadid_t *real_dst,
 *				    void *rcv_msg,
 *				    l4_ipc_reg_msg_t *rcv_reg,
 *				    l4_timeout_t timeout,
 *				    l4_msgdope_t *result);
 ****************************************************************************
 *
 */

PROC(l4_mips_ipc_chief_reply_and_wait)
	daddiu	sp,-112
	stack_c
	sd	a4, 88(sp)
	sd	a5, 96(sp)
	sd	a7, 104(sp)
	/* Note: *result is already on stack (at offset 120) */
	msg_to_reg(a3)


	move	a3, a6
	move	a6, a1
	move	a1, a3
	move	a4, a0
	ori	a0, a2, L4_IPC_DECEIT_MASK  /* send desc */
	ld	a2, 112(sp)
	move	a5, zero

	.set	noat
	dli	AT, SYSCALL_IPC
	syscall
	.set	at

	ld	a1, 88(sp)
	sd	v1, (a1)
	ld	a0, 96(sp)
	sd	a4, (a0)
	ld	a1, 104(sp)
	reg_to_msg(a1)
	ld	a0, 120(sp)
	sd	v0, (a0)
	unstack_c
	daddiu	sp, 112
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_chief_reply_and_wait)


/****************************************************************************
 * extern int
 * l4_mips_ipc_chief_send(l4_threadid_t dest,
 *			  l4_threadid_t vsend,
 *			  const void *snd_msg,
 *			  l4_ipc_reg_msg_t *snd_reg,
 *			  l4_timeout_t timeout,
 *			  l4_msgdope_t *result);
 ****************************************************************************/

PROC(l4_mips_ipc_chief_send)
	daddiu	sp,-96
	stack_c
	msg_to_reg(a3)
	sd	a5, 88(sp)

	move	a6, a1
	move	a5, a2
	move	a2, a4
	move	a4, a0
	ori	a0, a5, L4_IPC_DECEIT_MASK  /* send desc */
	dli	a1,  L4_IPC_NIL_DESCRIPTOR

	.set	noat
	dli	AT, SYSCALL_IPC
/*	dli	a5, 0 */
	syscall
	.set	at

	ld	a0, 88(sp)
	sd	v0, (a0)
	unstack_c
	daddiu	sp, 96
	andi	v0, L4_IPC_ERROR_MASK
	jr	ra
END(l4_mips_ipc_chief_send)
