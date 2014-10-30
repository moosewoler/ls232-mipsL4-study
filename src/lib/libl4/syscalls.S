/****************************************************************************
 *      $Id: syscalls.S,v 1.6 1998/12/29 05:23:42 gernot Exp $
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
#include <asm.h>
#include <regdef.h>
#include <l4/types.h>
#include <l4/syscalls.h>
#include "macros.h"

PROC(l4_thread_schedule)
	/* a0 dest
	 * a1 param
	 * a2 *ext_preempter
	 * a3 *partner
	 * a4 *old_param
	 * a5 *old_ext_preempter
	 */	

	.set noat
	daddi	sp, -112
	ld	a2, (a2)
	sd	a3, 88(sp)
	sd	a4, 96(sp)
	sd	a5, 104(sp)
	stack_c
	dli	AT, SYSCALL_THREAD_SCHEDULE
	syscall
	
	ld	s0, 88(sp)
	sd	a3, (s0)
	ld	s0, 96(sp)
	sd	a1, (s0)
	ld	s0, 104(sp)
	sd	a2, (s0)
	unstack_c
	daddi	sp, 112

	jr	ra
	.set at
END(l4_thread_schedule)

	
PROC(l4_myself)
	.set noat
	/* hmmm, to preserve C calling conventions we need to preserve
	and restore s0-7 registers and gp, fp, ra for system call */

	daddiu	sp, -88
	stack_c	
	move	a0, zero
	dli	AT, SYSCALL_ID_NEAREST
	syscall

	unstack_c
	daddiu	sp, 88	
	move	v0, v1
	jr	ra
	.set at
END(l4_myself)

PROC(l4_id_nearest)
	.set noat
	/* hmmm, to preserve C calling conventions we need to preserve
	and restore s0-7 registers and gp, fp, ra for system call */

	daddiu	sp, -96
	stack_c
	sd	a1, 88(sp)	
	dli	AT, SYSCALL_ID_NEAREST
	syscall
	ld	a1, 88(sp)
	sd	v1, (a1)
	unstack_c
	daddiu	sp, 96	
	jr	ra
	.set at
END(l4_id_nearest)



PROC(l4_thread_ex_regs)
	/* a0 lthread
	 * a1 new ip
	 * a2 new esp
	 * a3 * excpt
	 * a4 * pager
	 * a5 * old ip
	 * a6 * old sp
	 */
	.set noat
	/* hmmm, to preserve C calling conventions we need to preserve
	and restore s0-7 registers and gp, fp, ra for system call */

	daddiu	sp, -120
	stack_c	
	sd	a3, 88(sp)
	sd	a4, 96(sp)
	sd	a5, 104(sp)
	sd	a6, 112(sp)
	ld	a3, (a3)
	ld	a4, (a4)
	dsrl	a0, 10
	dli	AT, SYSCALL_LTHREAD_EX_REG
	syscall
	
	ld	s0, 88(sp)
	sd	a3, (s0)
	ld	s0, 96(sp)
	sd	a4, (s0)
	ld	s0, 104(sp)
	sd	a1, (s0)
	ld	s0, 112(sp)
	sd	a2, (s0)

	unstack_c
	daddi	sp, 120

	jr	ra
	.set at
END(l4_thread_ex_regs)

PROC(l4_thread_switch)
	/* a0 has thread id to switch to */
	.set noat
	/* hmmm, to preserve C calling conventions we need to preserve
	and restore s0-7 registers and gp, fp, ra for system call */

	daddi	sp, -88
	stack_c
	
	dli	AT, SYSCALL_THREAD_SWITCH
	syscall

	unstack_c
	daddi	sp, 88	

	jr	ra
	.set at
END(l4_thread_switch)

PROC(l4_task_new)
	/* args */
	/* a0 new tid */
	/* a1 mcp_or_new_chief */
	/* a2 esp */
	/* a3 eip */
	/* a4 pager */
	/* a5 excpt */
	/* kernel expects */
	/* a0 start address */
	/* a1 pager id */	
	/* a2 stack pointer*/
	/* a3 tid */
        /* a4 mcp_or_new_chief */
	/* a5 excpt */
	.set noat
	/* hmmm, to preserve C calling conventions we need to preserve
	and restore s0-7 registers and gp, fp, ra for system call */

	daddi	sp, -88
	stack_c

	move	s0, a0
	move	a0, a3
	move	a3, s0
	move	s0, a1
	move	a1, a4
	move	a4, s0
	dli	AT, SYSCALL_TASK_CREATE
	syscall

	unstack_c
	daddi	sp, 88	

	jr	ra
	.set at
END(l4_task_new)

PROC(l4_fpage_unmap)
	/* a0 fpage */
	/* a1 map mask */
	.set noat
	/* hmmm, to preserve C calling conventions we need to preserve
	and restore s0-7 registers and gp, fp, ra for system call */

	daddi	sp, -88
	stack_c
	
	dli	AT, SYSCALL_FPAGE_UNMAP
	syscall

	unstack_c
	daddi	sp, 88	

	jr	ra
	.set at
END(l4_fpage_unmap)
	
