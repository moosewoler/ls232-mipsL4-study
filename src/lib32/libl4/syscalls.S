/****************************************************************************
 *      $Id: syscalls.S,v 1.3 1999/03/08 07:35:28 gernot Exp $
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

#define ARG0  0
#define ARG0_32  4
#define ARG1  8
#define ARG1_32  12
#define ARG2  16
#define ARG3  24			
#define ARG3_32  28			
#define ARG4  32
#define ARG4_32  36
#define ARG5  40
#define ARG5_32  44
#define ARG6  48
#define ARG6_32  52
#define ARG7  56


PROC(l4_thread_schedule)
	/* a0 dest
	 * a1 param
	 * a2 * ext_preempter
	 * a3 * partner
	 * a4 * old_param
	 * a5 * old_ext_preempter
	 */
	/* hmmm, to preserve C calling conventions we need to preserve
	and restore s0-7 registers and gp, fp, ra for system call */

	addiu	sp, sp, -C_STACK
	STACK_C

	lw	a2, (a2)
	sw	a3, ARG3_32+C_STACK(sp)
	
	.set noat
	li	AT, SYSCALL_THREAD_SCHEDULE
	syscall
	.set at
	
	lw	s0, ARG3_32+C_STACK(sp)
	sd	a3, (s0)
	lw	s0, ARG4_32+C_STACK(sp)
	sd	a1, (s0)
	lw	s0, ARG5_32+C_STACK(sp)
	sd	a2, (s0)

	UNSTACK_C
	addiu	sp, sp, C_STACK

	jr	ra
END(l4_thread_schedule)

/****************************************************************************
 *
 * extern l4_threadid_t l4_myself(void); 
 *
 ****************************************************************************/
PROC(l4_myself)
	/* hmmm, to preserve C calling conventions we need to preserve
	and restore s0-7 registers and gp, fp, ra for system call */

	addiu	sp, sp, -C_STACK
	STACK_C

	sw	a0, ARG0_32+C_STACK(sp)
	move	a0, zero
	.set noat
	li	AT, SYSCALL_ID_NEAREST
	syscall
	.set at
	lw	a0, ARG0_32+C_STACK(sp)
	sd	v1, (a0)
	UNSTACK_C
	addiu	sp, sp, C_STACK
	jr	ra
END(l4_myself)
	
PROC(l4_thread_ex_regs)
	/* a0 lthread
	 * a1 new ip
	 * a2 new esp
	 * a3 * exception
	 * a4 * pager
	 * a5 * old ip
	 * a6 * old sp
	 */
	/* hmmm, to preserve C calling conventions we need to preserve
	and restore s0-7 registers and gp, fp, ra for system call */

	addiu	sp, sp, -C_STACK
	STACK_C
	sw	a3, ARG3_32+C_STACK(sp)
	ld	a3, (a3)
	lw	t0, ARG4_32+C_STACK(sp)
	ld	t0, (t0)
	dsrl	a0, 10

	.set noat
	li	AT, SYSCALL_LTHREAD_EX_REG
	syscall
	.set at
	
	lw	s0, ARG3_32+C_STACK(sp)
	sd	a3, (s0)
	lw	s0, ARG4_32+C_STACK(sp)
	sd	a4, (s0)
	lw	s0, ARG5_32+C_STACK(sp)
	sd	a1, (s0)
	lw	s0, ARG6_32+C_STACK(sp)
	sd	a2, (s0)

	UNSTACK_C
	addiu	sp, sp, C_STACK

	jr	ra
END(l4_thread_ex_regs)

/****************************************************************************
 *
 * l4_taskid_t 
 * l4_task_new(l4_taskid_t destination, dword_t mcp_or_new_chief, 
 * 	    dword_t esp, dword_t eip, l4_threadid_t pager, l4_threadid_t excpt)
 *
 ****************************************************************************/

PROC(l4_task_new)
	addiu	sp, sp, -C_STACK
	STACK_C
	sw	a0, ARG0_32+C_STACK(sp) /* struct pointer */

	move	t0, a2
	move	a2, a3
	move	a3, a1
	ld	a1, ARG5+C_STACK(sp)
	ld	a0, ARG4+C_STACK(sp)	
	ld	t1, ARG6+C_STACK(sp)	
	.set noat
	li	AT, SYSCALL_TASK_CREATE
	syscall
	.set at
	lw	a0, ARG0_32+C_STACK(sp)
	sd	v0, (a0)
	UNSTACK_C
	addiu	sp, sp, C_STACK
	jr	ra
END(l4_task_new)

/****************************************************************************
 * void l4_fpage_unmap(l4_fpage_t fpage, dword_t map_mask)
 ****************************************************************************/

PROC(l4_fpage_unmap)
	addiu	sp, sp, -C_STACK
	STACK_C

	.set	noat
	li	AT, SYSCALL_FPAGE_UNMAP
	syscall
	.set	at
	
	UNSTACK_C
	addiu	sp, sp, C_STACK
	jr	ra
END(l4_fpage_unmap)


/****************************************************************************
 * int l4_id_nearest(l4_threadid_t destination,
 * 		  l4_threadid_t *next_chief)
 ****************************************************************************/
	
PROC(l4_id_nearest)
	addiu	sp, sp, -C_STACK
	STACK_C

	sw	a1, ARG1_32+C_STACK(sp)
	
	.set	noat
	li	AT, SYSCALL_ID_NEAREST
	syscall
	.set	at
	
	lw	a1, ARG1_32+C_STACK(sp)
	sd	v1, (a1)
	
	UNSTACK_C
	addiu	sp, sp, C_STACK
	jr	ra
END(l4_id_nearest)

/****************************************************************************
 * void l4_thread_switch(l4_threadid_t destination)
 ****************************************************************************/
PROC(l4_thread_switch)
	addiu	sp, sp, -C_STACK
	STACK_C

	.set	noat
	li	AT, SYSCALL_THREAD_SWITCH
	syscall
	.set	at

	UNSTACK_C
	addiu	sp, sp, C_STACK
	jr	ra
END(l4_thread_switch)