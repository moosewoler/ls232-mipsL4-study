/****************************************************************************
 *      $Id: syscalls.S,v 1.6 2002/09/16 06:08:32 cls Exp $
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
#include <kernel/kernel.h>
#include <kernel/panic.h>
#include <asm.h>
#include <regdef.h>
#include <r4kc0.h>
#include <kernel/machine.h>
#include <l4/syscalls.h>
#include <l4/ipc.h>
#include "macros.h"

#ifdef INDY	
#define INITIAL_THREAD_ST (ST_KX|ST_UX|ST_SX|ST_IE|ST_KSU_U|ST_EXL)
#else
#define INITIAL_THREAD_ST (ST_KX|ST_UX|ST_SX|ST_IE|ST_KSU_U|ST_EXL)
#endif

#define L4_INVALID 0xffffffffffffffff
#define L4_INVALID_32 0x00000000ffffffff

#define SP_PRIORITY_MASK 0x00000000000000ff	
#define SP_TIME_MAN_SHIFT 24
#define SP_TIME_EXP_SHIFTL 40
#define SP_TIME_EXP_SHIFTR 60
#define SP_TSTATE_SHIFT 16

#define SP_RUNNING 0
#define SP_SENDING 4
#define SP_RECEIVING 8
#define SP_WAITING 0xc
#define SP_PENDING 0xd
#define SP_DEAD 0xf 

#define CT_CPU_TIME_MASK 0x0000ffffffffffff
#define CT_PF_WAKEUP_SHIFT SP_TSTATE_SHIFT+32
#define CT_TIME_MAN_SHIFT SP_TIME_MAN_SHIFT+32
#define CT_TIME_EXP_SHIFT SP_TIME_EXP_SHIFTR-SP_TIME_EXP_SHIFTL+32

PROC(syscall_init)
	lui	a0, KERNEL_BASE
	
	dla	a1, k_fpage_unmap
	sd	a1, (8*SYSCALL_FPAGE_UNMAP)+K_SYSCALL_JMP_TABLE(a0)
	
	dla	a1, k_id_nearest
	sd	a1, (8*SYSCALL_ID_NEAREST)+K_SYSCALL_JMP_TABLE(a0)

#ifdef 	MAP_INSTR
	dla	a1, set_map_vec
	sd	a1, (8*SYSCALL_ID_NCHIEF)+K_SYSCALL_JMP_TABLE(a0)
#else	
	dla	a1, syscall_error
	sd	a1, (8*SYSCALL_ID_NCHIEF)+K_SYSCALL_JMP_TABLE(a0)
#endif	
	dla	a1, k_thread_switch
	sd	a1, (8*SYSCALL_THREAD_SWITCH)+K_SYSCALL_JMP_TABLE(a0)

	dla	a1, k_thread_schedule
	sd	a1, (8*SYSCALL_THREAD_SCHEDULE)+K_SYSCALL_JMP_TABLE(a0)
	
	dla	a1, k_lthread_ex_reg
	sd	a1, (8*SYSCALL_LTHREAD_EX_REG)+K_SYSCALL_JMP_TABLE(a0)
	
	dla     a1, create_thread
	sd	a1, (8*SYSCALL_TASK_CREATE)+K_SYSCALL_JMP_TABLE(a0)

	jr	ra
END(syscall_init)
	.data
sys_msg:	
	.asciiz "L4 PANIC: Unimplemented system call"

PROC(syscall_error)
	dla	a0, sys_msg
	j	panic /* ok */
END(syscall_error)


	.data
msg:	
	.asciiz "cthr"
no_asid_msg:
	.asciiz "no asid's left"

cv_msg:
	.asciiz "attempt to create already valid thread"
nov_msg:
	.asciiz "L4 PANIC: All possible version numbers for new task used"
cc_msg:
	.asciiz "change chief not implemented"
dp_msg:
	.asciiz "L4 PANIC: Maximum depth of task hierarchy exceeded"
lost_thread_msg:
	.asciiz "can't find thread to remove"

PROC(k_thread_schedule)
	/* in
	 *
	 * a0 dest
	 * a1 param
	 * a2 ext preempter
	 *
	 * out
	 *
	 * a1 old_param
	 * a2 old ext_preempter
	 * a3 partner
	 */

	move	s0, a0
	move	s1, a1
	move	s2, a2
	tcbtop(s5)

	/* check if destination thread is valid */
	tid2tcb(s0, s6) /* s6 contains destination thread tcb base */
	lw	t0, T_COARSE_STATE(s6)
	andi	t0, t0, CS_INVALID_TCB
	bne	t0, zero, ts_inv /* invalid destination thread */

	/* check current task's mcp against priority of destination task */
	/* NOTE: using task mcp not thread mcp */	

	/* get current task's mcp */
	ld	t0, T_MYSELF-TCBO(s5)
	li	t1, TID_TASK_MASK	
	and	t0, t0, t1
	tid2tcb(t0, t1) 
	lbu	s7, T_MCP(t1) /* s7 contains current task's mcp */

	/* get destination thread's priority */
	lbu	t1, T_TSP(s6) /* t1 contains destination thread's priority */
	
	/* NOTE: currently ok if mcp and destination priority are equal */
	subu	t0, s7, t1
	bltz	t0, ts_inv /* mcp violation */ 	

	/* store old priority */
	lbu 	t9, T_TSP(s6)
	dli	a4, 0 /* initialise old_param */
	or	a4, a4, t9

	/* store old timeslice */
	lhu	t9, T_TIMESLICE(s6)
	encode_time(t9, t1, t2)
	sll	t1, t1, SP_TIME_MAN_SHIFT
	or	a4, a4, t1
	sll	t2, t2, (SP_TIME_EXP_SHIFTR - SP_TIME_EXP_SHIFTL)
	or	a4, a4, t2

	/* store destination thread's state and partner*/
	lwu	t0, T_FINE_STATE(s6)
	andi	t1, t0, FS_BUSY
	beq	zero, t1, 1f	
	dli	t2, SP_RUNNING
	b	6f
1:	andi	t1, t0, FS_LOCKS
	beq	zero, t1, 2f	
	dli	t2, SP_SENDING
	
	ld	t0, T_COMM_PARTNER(s6)
	ld	a3, T_MYSELF(t0)
	
	b	6f
2:	andi	t1, t0, FS_LOCKR
	beq	zero, t1, 3f	
	dli	t2, SP_RECEIVING

	ld	a3, T_WFOR(s6)

	b	6f	
3:	andi	t1, t0, FS_WAIT
	beq	zero, t1, 4f	
	dli	t2, SP_WAITING

	dli	a3, L4_INVALID_ID

	wakeup_remaining(s6, s2, t1)	
	b	6f
4:	andi	t1, t0, FS_POLL
	beq	zero, t1, 5f	
	dli	t2, SP_SENDING

	ld	t0, T_COMM_PARTNER(s6)
	ld	a3, T_MYSELF(t0)

	wakeup_remaining(s6, s2, t1)
	b	6f
5:	dli	t2, SP_DEAD
	b	6f		

6:	sll	t2, t2, SP_TSTATE_SHIFT
	or	a4, a4, t2 /* thread state */

	/* check if 'param' is the invalid value */
	dli	t0, L4_INVALID_32
	and	t1, s1, t0
	beq	t0, t1, ts_ret 

	/* update new timeslice */
	move	t0, s1 /* t0 now contains 'param' */
	srl	t1, t0, SP_TIME_MAN_SHIFT /* t1 contains timeslice mantissa */
	dsll	t2, t0, SP_TIME_EXP_SHIFTL
	dsrl	t2, t2, SP_TIME_EXP_SHIFTR /* t2 contains timeslice exponent */
	decode_time(t1, t2, a1)	
	sh	a1, T_TIMESLICE(s6)

	/* check if priority in 'param' <= mcp of current thread */
	and	t0, s1, SP_PRIORITY_MASK /* t0 contains priority in 'param' */
	subu	t0, s7, t0
	bltz	t0, ts_ret /* can't make others' prio. higher than own mcp */

	and	t0, s1, SP_PRIORITY_MASK /* t0 contains new priority */

	/* remove thread from old busy list 
	 *  (remove only if in one but not the right one or timeslice = 0)
	 *  (s6 contains destination thread tcb base) 
	 */
	lbu	t1, T_TSP(s6)
	sb	t0, T_TSP(s6) /* update new priority */

	ld	t2, T_BUSY_LINK(s6)
	bne	zero, t2, ts_rem_bl1 /* in busy list so remove */

	/* busy but not in busy list - only occurs when old timeslice is 0 */
	lw	t2, T_FINE_STATE(s6)
	andi	t2, t2, FS_BUSY
	bne	zero, t2, ts_ins_bl 
	b	ts_ret

ts_rem_bl1:
	beq	zero, a1, ts_rem_bl2 /* zero timeslice so remove regardless */
	beq	t0, t1, ts_ret /* tsp unchanged - nothing to do */
ts_rem_bl2:
	sll	t1, t1, 3 
	lui	t0, KERNEL_BASE
	daddu	t1, t1, t0 /* t1 contains offset to correct list */
	ld	t2, K_PRIO_BUSY_LIST(t1)
	beq	zero, t2, rs_err /* error: list is empty */
	ld	t3, T_BUSY_LINK(t2) /* t3 is next tcb pointer */
	move	t0, t2 /* t0 is prev tcb pointer */
rs_loop:	
	beq	t3, s6, rs_found /* found thread - remove it now */
	beq	t3, t2, rs_err /* thread not in list */
	move	t0, t3 /* prev = next */
	ld	t3, T_BUSY_LINK(t3) /* next = next->next */	
	b	rs_loop
rs_found:
	beq	t0, s6, rs_only /* check if queue only contains dest. thread */
	ld	t2, K_PRIO_BUSY_LIST(t1)
	bne	t2, s6, rs_rem /* check if thread is at tail of list */
	sd	t0, K_PRIO_BUSY_LIST(t1)
rs_rem:
	ld	t2, T_BUSY_LINK(t3)
	sd	t2, T_BUSY_LINK(t0)
	b	rs_end
rs_only:
	/* list only contains destination thread */
	sd	zero, K_PRIO_BUSY_LIST(t1)
rs_end:
	sd	zero, T_BUSY_LINK(s6)
ts_ins_bl: 
	/* insert into new list if timeslice is non-zero */
	beq	zero, a1, ts_ret /* a1 is the new timeslice (from above) */
	lui	t0, KERNEL_BASE
	ins_busy_list(s6, t0, t1) 
	b	ts_ret
rs_err:
	dla	a0, lost_thread_msg
	j	panic	/* ok */

	/* invalid destination thread or mcp violation */
ts_inv:	dli	a1, L4_INVALID
	b	ts_ret1
	
	
ts_ret:	move	a1, a4 /* quick fix: old_param changed from a4 to a1 */
ts_ret1:
	/* get return parameter and return */
        ld      v0, T_CPU_TIME(s6) /* cpu time */
        and     v0, v0, CT_CPU_TIME_MASK
        dli     t0, 0xfff0000000000000
        and     s2, s2, t0
        or      v0, v0, s2 /* wakeup  remaining */
        syscall_ret 
END(k_thread_schedule)


	/* syscalls have KERNEL_BASE in k0
	   t8  now has source tcb base + TCBO (TCB offset) */
PROC(create_thread)
	/* args are 
	 *
	 * 	a0 initial IP
	 * 	a1 PAGER
	 * 	a2 user sp
	 * 	a3 tid
	 *	a4 mcp / new_chief
	 *      a5 excpt handler
	 */
	/* put args in save registers so we can call C */
	move	s0, a0
	move	s4, a1     /* a1 has pager id */
	move	s5, a2
	move	s6, a3
	move	s2, a4
	move	gp, a5

	trace(crth)

	/* first check validity of task */

	tid2ttable(s6, t0)
	lw	t1, (t0)
	li	t2, TT_INACTIVE_MASK
	and	t2, t2, t1
	beq	zero, t2, active_task

	/* we have invalid task, check chief okay */
the_after_life:
	li	t2, TT_CHIEF_MASK
	and	t3, t2, t1
	tcbtop(a0)
	ld	a1, T_MYSELF-TCBO(a0)
	beq	t3, zero, 1f
	
	xor	a2, t1, a1
	and	a2, a2, t2
	beq	a2, zero, 1f

	/* chief mismatch -> permision denied */
	
	move	v0, zero
	b	ct_ret
	
	
1:	/* chief okay */
	
	/* check pager */
	bne	s4, zero, 1f
	
	/* pager invalid, change chief and return */

	li	a3, TID_TASK_MASK
	and	a6, s2, a3 /* a6 has new chief */
	li	a4, ~TT_CHIEF_MASK
	and	a5, t1, a4
	or	a5, a5, a6
	sw	a5, (t0)   /* new chief stored in task table */

	dsll	v0, a6, 32
	and	v1, s6, a3
	or	v0, v0, v1
	
	b	ct_ret
	
1:	/* pager valid */
			
	/* inc version number*/
	addiu	t2, t1, 1
	andi	t3, t2, TT_OVRFLW_MASK
	beq	zero, t3, 1f
	/* ran out of versions */
	dla	a0, nov_msg
	j	panic /* ok */

1:	/* mask out invalid bit in t table */
	li	t1, 0x7fffffff
	and	t2, t2, t1
	sw	t2, (t0)

	andi	t1, t2, 01777 
	srl	t0, t2, 10
	andi	t0, t0, 017
	sll	t0, t0, 28
	or	t2, t1, t0    /* t2 has correct new version number in right
				place */
	
	/* combine with task id */
	
	dli	t0, TID_TASK_MASK
	and	s7, t0, s6
	or	s7, s7, t2    /* s7 has new thread id */

	/* combine with chief */
	and	s6, a1, t0
	dsll	s6, s6, 32
	or	s7, s7, s6

	/* set depth FIXME: > 15 */
	dsrl	s6, a1, 60
	daddiu	s6, s6, 1
	andi	t2, s6, 020
	beq	t2, zero, 1f

	dla	a0, dp_msg
	j	panic /* FIXME!!! */

1:		
	dsll	s6, s6, 60
	or	s7, s7, s6
	
	 /* s7 has new thread id (inc chief and depth) */
	
	/* FIXME:  site */

	tid2tcb(s7, s6) /* s6 has new tcb vaddress */

	jal	tcb_frame_alloc /* alloc a new frame for tcb */

	tcbtop(t9)
	ld	a0, T_GPT_POINTER-TCBO(t9)
	move	a1, s6
	move	a2, v0

	jal	vm_tcb_insert

	daddiu	s1, s6, TCB_SIZE /* s1 now contains top of new stack */
	move	s8, s1  /* base of second gpt in pair */
	daddiu	t2, s8, TCB_SIZE
	
	/* now build a stack to switch to */
	dli	t0, -1
	sd	s5, -8(s1)	/* new thread sp */
	sd	t0, -8(t2)
	sd	s0, -16(s1)	/* new thread start address */
	sd	t0, -16(t2)
	li	t1, INITIAL_THREAD_ST
	sb	t1, -24(s1)
	sb	t1, -24(t2)

	daddiu	s1,s1,-24

	/* initialise most tcb vars */

	init_tcb(s6)
	init_tcb(s8)
	
	sd	s7, T_MYSELF(s6)
	daddiu	t0, s7, 1 << 10
	sd	t0, T_MYSELF(s8)
	
	/* a4 contains the system call mcp
	 * a1 will contain creator mcp 
	 */
	tcbtop(t9)
	lbu	a1, T_MCP-TCBO(t9) 
	sub	t2, a1, s2 
	blez	t2, 1f		/* if (creator.mcp > call.mcp) */
	move	t2, s2		/*	new.mcp = call.mcp */
	b	2f
1:	move	t2, a1		/* else new.mcp = creator.mcp */
2:	sb	t2, T_MCP(s6)

	lbu	t2, T_TSP-TCBO(t9)
	sb	t2, T_CTSP(s6)	/* dest.ctsp = src.tsp */
	sb	t2, T_TSP(s6)	/* dest.tsp = src.tsp */
	/*sb	t2, T_MCP(s8)
	sb	t2, T_CTSP(s8)*/	

	lhu	t2, T_TIMESLICE-TCBO(t9)
	sh	t2, T_TIMESLICE(s6)	/* new.timeslice = creator.timeslice */
	sh	t2, T_REM_TIMESLICE(s6) /* new.rem_timeslice = */
	/*sh	t2, T_TIMESLICE(s8)*/	/*	creator.timeslice */
	/*sh	t2, T_REM_TIMESLICE(s8)*/

	/* init the gpt */
	move	a0, s6
	jal	vm_new_as
	ld	v0, T_GPT_POINTER(s6)
	sd	v0, T_GPT_POINTER(s8)
	sd	s4, T_PAGER_TID(s6)
	sd	s4, T_PAGER_TID(s8)
	sd	gp, T_EXCPT_TID(s6)
	sd	gp, T_EXCPT_TID(s8)

	/* allocate an asid */
	move	a0, s7
	jal	asid_alloc /* asid alloc uses t0, v0, AT, ra */

	sd	v0, T_ASID(s6)
	sd	v0, T_ASID(s8)
	

	/* add new thread to run queue */
	sd	s1, T_STACK_POINTER(s6)
	lui	a2, KERNEL_BASE
	ins_busy_list(s6, a2, t0)

	/* init new present list for this task */
	tcbtop(t9)
	daddiu	s3, t9, -TCBO

	sd	s8, T_PRESENT_NEXT(s6)
	sd	zero, T_PRESENT_NEXT(s8)

	/* add task as child of this task, and move current child to sister
	of new task */
	tcbtop(t0)
	ld	t0, T_MYSELF-TCBO(t0)
	dli	t1, TID_TASK_MASK
	and	t0, t0, t1    
	tid2tcb(t0, t1) 
	ld	t0, T_CHILD_TASK(t1)
	sd	t0, T_SISTER_TASK(s6)
	sd	s6, T_CHILD_TASK(t1)
	beq     t0, zero, 3f
	sd      s6, T_SISTER_TASK_PREV(t0)
3: 
	
	/* set running state */
	li	t2, FS_BUSY
	sw	t2, T_FINE_STATE(s6)
	li	t2, FS_INACTIVE
	sw	t2,  T_FINE_STATE(s8)

	/* stack state for parent return */
	daddiu	sp, sp, -16
	dla	t0, parent_thread_restart
	sd	s7, 8(sp)
	sd	t0, (sp)


	/* make sure parent is in busy list */
	ins_busy_list(s3, a2, t0)
	
	tcbtop(t9)
	thread_switch_fast(t9, s6, a2)
	trace(ecrt)

1:
ct_ret:	
	syscall_ret 


active_task:
	trace(kill)

	/* test if chief */
	dli	t3, TID_TASK_MASK
	and	t1, s6, t3	
	tid2tcb(t1,s1)
	ld	t1, T_MYSELF(s1)
	tcbtop(a0)
	ld	a1, T_MYSELF-TCBO(a0)
	dsrl	t2, t1, 32
	xor	t2, t2, a1
	and	t2, t2, t3
	beq	t2, zero, 1f

	/* not chief, return */
	move	v0, zero
	b	ct_ret

1:	
	/* chief okay, check if task_new already running */
	lw	t1, T_FINE_STATE(s1)
	andi	t2, t1, FS_DYING
	beq	t2, zero, 1f

	/* task already dying */
	
	move	v0, zero
	b	ct_ret

1:	/* okay lets kill the task */
	trace(kil2)
	/* mark as dying */
	li	t0, ~(FS_BUSY | FS_WAKEUP)
	and	t1, t1, t0
	ori	t1, t1, FS_DYING
	sw	t1, T_FINE_STATE(s1)
	sd	zero, T_MYSELF(s1)
	
	/* now null FINE_STATE and MYSELF of threads in task
	to prevent struggling while killing */

	ld	t1, T_PRESENT_NEXT(s1)
	beq	t1, zero, 1f
	li	t2, ~(FS_BUSY | FS_WAKEUP)
2:	
	lw	t0, T_FINE_STATE(t1)
	and	t0, t0, t2
	ori	t0, t0, FS_DYING
	sw	t0, T_FINE_STATE(t1)
	sd	zero, T_MYSELF(t1)
	ld	t1, T_PRESENT_NEXT(t1)
	bne	t1, zero, 2b
	
	/* now task and thread are unrunnable and invalid */

	/* remove non-busy tcb's from busy_list and
	non-wake tcbs from wake lists */

1:	trace(kil3)

	jal	process_lists
	trace(kil4)
	/* remove if polling */
	move	a0, s1
2:	lw	t1, T_FINE_STATE(a0)
	andi	t1, t1,	FS_POLL	
	beq	t1, zero, 1f

	ld	t0, T_COMM_PARTNER(a0)
	/* we are polling */
	rem_sendq(a0, t0, t1)
		
1:	trace(kil5)	
	/* now break off threads pending for this thread */
	ld	t0, T_SNDQ_START(a0)
	beq	t0, zero, 1f

	/* we have a pending thread */
	/* remove from pending queue */

	rem_sendq(t0, a0, t1)
	/* restart them if they are not dying as well */
	lw	t3, T_FINE_STATE(t0)
	andi	t3, t3, FS_DYING
	bne	t3, zero, 1b

	dla	t1, pending_recv_killed
	ld	t2, T_STACK_POINTER(t0)
	sd	t1, (t2)

	li	t2, FS_BUSY
	sw	t2, T_FINE_STATE(t0)
	lui	t3, KERNEL_BASE
	ins_busy_list(t0, t3, t2)

	b	1b            /* do any remaining in queue */

1:	trace(kil6)
	/* check if LOCK (in fine state ) */
	lw	t0, T_FINE_STATE(a0)
	andi	t1, t0, FS_LOCKS
	beq	t1, zero, 4f

	ld	t1, T_COMM_PARTNER(a0)
	lw	t2, T_FINE_STATE(t1)
	andi	a1, t2, FS_POLL
	beq	a1, zero, 5f /* check if partner POLL (rcv pf) */

	ld	t3, T_COMM_PARTNER(t1)
	rem_sendq(t1, t3, a2)
	
	/* assume partner is LOCKR and make busy if not dying */
5:	andi	a1, t2, FS_DYING
	bne	a1, zero, 1f
	li	a2, L4_IPC_REABORTED
	make_busy(t1, a2)
	lui     t3, KERNEL_BASE
	ins_busy_list(t1, t3, t2)
	sw	zero, T_STACKED_FINE_STATE(t1)
	b	1f
	
4:	/* test if LOCKR */
	andi	t1, t0, FS_LOCKR
	beq	t1, zero, 4f

	ld	t1, T_COMM_PARTNER(a0)
	lw	t2, T_FINE_STATE(t1)
	andi	a1, t2, FS_POLL
	beq	a1, zero, 5f /* check if partner POLL (rcv pf) */

	ld	t3, T_COMM_PARTNER(t1)
	rem_sendq(t1, t3, a2)
	
	/* assume partner is LOCKS and make busy if not dying */
5:	andi	a1, t2, FS_DYING
	bne	a1, zero, 1f
	li	a2, L4_IPC_SEABORTED
	make_busy(t1, a2)
	lui     t3, KERNEL_BASE
	ins_busy_list(t1, t3, t2)
	sw	zero, T_STACKED_FINE_STATE(t1)
	b	1f
	

4:	/* test stacked state */

	lw	t0, T_STACKED_FINE_STATE(a0)
	andi	t1, t0, FS_LOCKS
	beq	t1, zero, 4f

	ld	t1, T_STACKED_COMM_PRTNR(a0)
	lw	t2, T_FINE_STATE(t1)
	andi	a1, t2, FS_DYING
	bne	a1, zero, 1f
	li	a2, L4_IPC_REABORTED
	make_busy(t1, a2)
	lui     t3, KERNEL_BASE
	ins_busy_list(t1, t3, t2)
	sw	zero, T_STACKED_FINE_STATE(t1)
	b	1f

4:
	andi	t1, t0, FS_LOCKR
	beq	t1, zero, 1f

	ld	t1, T_STACKED_COMM_PRTNR(a0)
	lw	t2, T_FINE_STATE(t1)
	andi	a1, t2, FS_DYING
	bne	a1, zero, 1f
	li	a2, L4_IPC_SEABORTED
	make_busy(t1, a2)
	lui     t3, KERNEL_BASE
	ins_busy_list(t1, t3, t2)
	sw	zero, T_STACKED_FINE_STATE(t1)

1:	trace(kil7)
	/* FIXME: any more thread specific cleanup goes here */
	
	ld	a0, T_PRESENT_NEXT(a0)
	bne	a0, zero, 2b

	/* now cleanup task specific stuff */

	/* unmap gpt */
	move	a0, s1
	dli	a1, 63 << 2
	dli	a2, -1

	jal	vm_fpage_unmap

	trace(kil8)
	move	a0, s1
	jal	vm_delete_as
	

	/* flush asid from tlb (remove window mappings) */
	ld	a0, T_ASID(s1)
	bltz	a0, 7f
	jal	tlb_flush_asid

	/* don't need to flush the STLB as
		- user pages are removed via the fpage_unmap
		- window pages are never placed in the STLB
		- tcb pages are global and thus are ASID independent */
#if 0
	ld	a0, T_ASID(s1)
	jal	tlb_cache_flush_asid
#endif
	/* free ASID */

	ld	a0, T_ASID(s1)
	jal	asid_free

	/* remove from current process hierarchy */
7:	tcbtop(t0)
	ld      t1, T_MYSELF-TCBO(t0)
	dli     t0, TID_TASK_MASK
	and     t1, t1, t0
	tid2tcb(t1,t0)
	ld      t1, T_CHILD_TASK(t0)
	ld      t2, T_SISTER_TASK(s1)
	bne     t1, s1, 8f  /*if deleting first */
	nop
	sd      t2, T_CHILD_TASK(t0)
	beq     t2, zero, 9f
	nop
	sd      zero, T_SISTER_TASK_PREV(t2) /* this is not really needed (just tidy) */
	b       9f
	nop
8:	ld      t0, T_SISTER_TASK_PREV(s1)
	sd      t2, T_SISTER_TASK(t0)
	beq     t2, zero, 9f
	nop
	sd      t0, T_SISTER_TASK_PREV(t2)
9:             
	
trace(kil9)
/****************************************************************************
 * loop through children nailing them 
 *
 */
#define start s1
#define x     s7

	move	x, start

1:		
	/* while (x != start || x->child != 0) */
	bne	x, start, 2f
	ld	t0, T_CHILD_TASK(x)
	beq	t0, zero, 3f
2:
	/* while (x->child != 0)
	{
		  x = x->child;
	}
	*/
	ld	t0, T_CHILD_TASK(x)
	beq	t0, zero, 4f

	move	x, t0
	b	2b
	
4:	
	/* x->parent->child = x->sister; */
	ld	t0, T_MYSELF(x)
	dsrl	t0, t0, 32
	dli	t1, TID_TASK_MASK
	and	t0, t0, t1    
	tid2tcb(t0, s8) /* s8 now has parent */
	ld	t1, T_SISTER_TASK(x)
	sd	t1, T_CHILD_TASK(s8)

	/***** start clean up */

		/* mark as dying */
	li	t0, ~(FS_BUSY | FS_WAKEUP)
	and	t1, t1, t0
	ori	t1, t1, FS_DYING
	sw	t1, T_FINE_STATE(x)
	ld	s3, T_MYSELF(x)
	sd	zero, T_MYSELF(x)
	
	/* now null FINE_STATE and MYSELF of threads in task
	to prevent struggling while killing */

	ld	t1, T_PRESENT_NEXT(x)
	beq	t1, zero, 6f
	li	t2, ~(FS_BUSY | FS_WAKEUP)
5:	
	lw	t0, T_FINE_STATE(t1)
	and	t0, t0, t2
	ori	t0, t0, FS_DYING
	sw	t0, T_FINE_STATE(t1)
	sd	zero, T_MYSELF(t1)
	ld	t1, T_PRESENT_NEXT(t1)
	bne	t1, zero, 5b
	
	/* now task and thread are unrunnable and invalid */

	/* remove non-busy tcb's from busy_list and
	non-wake tcbs from wake lists */
6:
	jal	process_lists

	/* remove shared branch of tree in dying task*/
	
	move	a0, x
	jal	vm_delete_as


	/* flush asid from tlb (remove window mappings) */
	ld	a0, T_ASID(x)
	bltz	a0, 7f
	jal	tlb_flush_asid
	
	/* don't need to flush the STLB as
		- user pages are removed via the fpage_unmap
		- window pages are never placed in the STLB
		- tcb pages are global and thus are ASID independent */
#if  0
	ld	a0, T_ASID(s1)
	jal	tlb_cache_flush_asid
#endif

	/* free ASID */
	ld	a0, T_ASID(x)
	jal	asid_free

7:	trace(ki10)
	/* set new chief to initial killer in process hierarchy */	
	tid2ttable(s3, t0)
	lw	t1, (t0)
	tcbtop(a0)
	ld	a1, T_MYSELF-TCBO(a0)
	li	t2, ~TT_CHIEF_MASK
	and	t1, t2, t1
	li	t2, TT_INACTIVE_MASK
	or	t1, t2, t1
	li	t2,  TID_TASK_MASK
	and	a1, a1, t2
	or	t1, a1, t1
	sw	t1, (t0)
	
	ld	a0, T_GPT_POINTER-TCBO(a0)
	move	a1, x
	
	jal	vm_tcb_unmap
	
	/***** end clean up */	
	/* x =  x->parent; */
	move	x, s8 
	b	1b
3:		
	/* end while */
	
/****************************************************************************
 */	
	/* FIXME: free tcbs */		

	tcbtop(t0)
	ld	a0, T_GPT_POINTER-TCBO(t0)
	move	a1, s1

	jal	vm_tcb_unmap
	trace(ekil)
	tid2ttable(s6, t0)
	lw	t1, (t0)
	li	t2, TT_INACTIVE_MASK
	or	t1, t2, t1
	sw	t1, (t0)

	j	the_after_life
END(create_thread)

PROC(parent_thread_restart)
	ld	v0, 8(sp)
	daddiu	sp, sp, 16
	syscall_ret 
END(parent_thread_restart)
	
	
	
	
	
	/* syscalls have KERNEL_BASE in k0
	   t8  now has source tcb base + TCBO (TCB offset) */
PROC(k_id_nearest)
	.set	noreorder
	bne	a0, zero, 1f
	ld	v1, T_MYSELF-TCBO(t8)
	.set	reorder
	syscall_ret 
1:	jal	nchief
	sw	zero, -8(sp)	/* fix from `Inside L4' document */
	syscall_ret 	
END(k_id_nearest)


	/* syscalls have KERNEL_BASE in k0
	   t8  now has source tcb base + TCBO (TCB offset) */
PROC(k_thread_switch)
	trace(tdsw)
	cbreak 
	/* get tcb base */
	daddiu	t1, t8, -TCBO

	/* stack ready for restart */
	daddiu	sp, sp , -8
	dla	t0, k_thread_switch_restart
	sd	t0, (sp)
	
	/* make sure caller in busy list */
	lui	t2, KERNEL_BASE
	ins_busy_list(t1,t2,t3)

	beq	a0, zero, 1f /* check if "open" switch */

	tid2tcb(a0, a1)
	lw	t0, T_FINE_STATE(a1)
	andi	t0, t0, FS_BUSY
	beq	t0, zero, 1f

	thread_switch_fast(t8, a1, t2)
	ld	ra,(sp)
	jr	ra
1:
	to_next_thread(t2)
END(k_thread_switch)

PROC(k_thread_switch_restart)
	trace(tswr)
	cbreak 
	daddiu	sp,sp,8
	syscall_ret 
END(k_thread_switch_restart)

	/* syscalls have KERNEL_BASE in k0
	   t8  now has source tcb base + TCBO (TCB offset) */
PROC(k_fpage_unmap)
	/* a0 has fpage */
	/* a1 has mask */
	move	a2, a1
	move	a1, a0
	daddiu	a0, t8, -TCBO
#ifdef MAP_INSTR
	.set	noreorder
	mfc0	s0, C0_COUNT
	nop
	.set	reorder
#endif		
	jal	vm_fpage_unmap
	/* registers are now trashed */

#ifdef MAP_INSTR
	.set noreorder
	mfc0	s1, C0_COUNT
	lui	t0, KERNEL_BASE
	ld	t1, K_TLB_MISS(t0)
	beq	t1, zero, 1f
	dsubu	t2, s1, s0
	sd	t2, (t1)
1:	
	.set	reorder
#endif		

	
	syscall_ret 
END(k_fpage_unmap)

	.data
msg_tcb_state:	
	.asciiz "L4_PANIC: Found TCB in unknown state"

PROC(k_lthread_ex_reg)
	trace(exrg)
	/* in 
	 *
	 * a0 lthread
	 * a1 new ip
	 * a2 new esp
	 * a3 excpt
	 * a4 pager
	 *
	 * out 
	 *
	 * a1 old ip
	 * a2 old esp
	 * a3 old excpt
	 * a4 old pager
	 */
	cbreak 
	move	s4, a1
	move	s5, a2
	move	s6, a3
	move	s7, a4

	/* syscalls have KERNEL_BASE in k0
	   t8  now has source tcb base + TCBO (TCB offset) */
	ld	s0, T_MYSELF-TCBO(t8)
	andi	a0, a0, 0177 /* lthread mask */
	dsll	a0, a0, 10
	dli	s1, ~(0177 << 10)
	and	s1, s0, s1
	or	s1, s1, a0 /* s1 has new thread id */

	tid2tcb(s1, s2) /* s2 now has new tcb base */

	lw	t1, T_COARSE_STATE(s2)
	andi	t0, t1, CS_INVALID_TCB
	beq	t0, zero, 1f

	/* allocate a valid tcb */
	jal	tcb_frame_alloc /* alloc a new frame for tcb */

	tcbtop(s3)
	ld	a0, T_GPT_POINTER-TCBO(s3)
	dli	t0, ~(L4_PAGESIZE-1)
	and	a1, s2, t0
	move	a2, v0

	/* brk */
	
	jal	vm_tcb_insert


	/* brk */

	/* put base (s3) and tid (s8) for other in pair */
	andi	t0, s2, TCB_SIZE
	bne	t0, zero, 2f
	
	daddiu	s3, s2, TCB_SIZE
	daddiu	s8, s1, 1 << 10
	b	3f

2:
	daddiu	s3, s2, -TCB_SIZE
	daddiu	s8, s1, -(1 << 10)


3:
	.set noreorder
	
	/* now initialise both TCBs in pair */
	
	daddiu	t3, s2, TCB_SIZE /* s1 now contains top of new stack */
	daddiu	t2, s3, TCB_SIZE

	dli	t0, -1
	sd	t0, -8(t3)	/* new thread sp */
	sd	t0, -8(t2)	/* new thread sp */
	sd	t0, -16(t3)	/* new thread start address */
	sd	t0, -16(t2)	/* new thread start address */
	li	t1, INITIAL_THREAD_ST
	sb	t1, -24(t3)
	sb	t1, -24(t2)

	init_tcb(s2)
	init_tcb(s3)
	tcbtop(t8)
	li	t0, FS_INACTIVE
	sw	t0, T_FINE_STATE(s2)
	sw	t0, T_FINE_STATE(s3)
	
	sd	s1, T_MYSELF(s2)
	sd	s8, T_MYSELF(s3)

	ld	t0, T_GPT_POINTER-TCBO(t8)
	sd	t0, T_GPT_POINTER(s2)
	sd	t0, T_GPT_POINTER(s3)

	ld	t0, T_PAGER_TID-TCBO(t8)
	sd	t0, T_PAGER_TID(s2)
	sd	t0, T_PAGER_TID(s3)

	ld	t0, T_EXCPT_TID-TCBO(t8)
	sd	t0, T_EXCPT_TID(s2)
	sd	t0, T_EXCPT_TID(s3)
	ld	t0, T_ASID-TCBO(t8)
	sd	t0, T_ASID(s2)
	sd	t0, T_ASID(s3)

	ld	t3, T_PRESENT_NEXT-TCBO(t8)
	sd	t3, T_PRESENT_NEXT(s3)
	sd	s3, T_PRESENT_NEXT(s2)
	sd	s2, T_PRESENT_NEXT-TCBO(t8)

1:
	/* set scheduling variables if first creation */
	lw	t0, T_FINE_STATE(s2)
	andi	t0, t0, FS_INACTIVE
	beq	t0, zero, 1f /* not 1st creation: no sched. change */

	lhu	t0, T_TIMESLICE-TCBO(t8)
	sh	t0, T_REM_TIMESLICE(s2)
	sh	t0, T_TIMESLICE(s2)	

	/* following lines not needed: only lthread0 needs valid mcp */
	/*lbu	t0, T_MCP-TCBO(t8)
	sb	t0, T_MCP(s2)*/
	
	lbu	t0, T_TSP-TCBO(t8)
	sb	t0, T_CTSP(s2)
	sb	t0, T_TSP(s2)	

1:	/* now we can actually do the ex_reg as we have a valid
	tcb */
	daddiu	t3, s2, TCB_SIZE 
	dli	t0, -1
	move	s8, zero
	.set noreorder
	beq	s7, t0, 1f
	ld	a4, T_PAGER_TID(s2)
	sd	s7, T_PAGER_TID(s2)
1:	beq	s6, t0, 1f
	ld	a3, T_EXCPT_TID(s2)
	sd	s6, T_EXCPT_TID(s2)
1:	beq	s5, t0, 1f
	ld	a2, -8(t3)
	sd	s5, -8(t3)
	move	s8, t0
1:	beq	s4, t0, 4f
	ld	a1, -16(t3)
	sd	s4, -16(t3)
	move	s8, t0
	.set	reorder

	/* (re)starting a thread */
	
	/* first break of anything pending for 
	current incarnation of thread FIXME: optimise a little */
	
4:	ld	t0, T_SNDQ_START(s2)
	lui	a5, KERNEL_BASE
	beq	t0, zero, 3f
	/* we have a pending thread */
	/* remove from pending queue */
	rem_sendq(t0, s2, t1)
	/* restart them */
	dli	v0, L4_IPC_SECANCELED
	make_busy(t0, v0)
	ins_busy_list(t0, a5, t2)
	b	4b            /* do any remaining in queue */

		

3:	/* can now assume no threads pending for this thread */

	lui	a5, KERNEL_BASE
	lw	s0, T_FINE_STATE(s2)
	
	/* case: LOCKS and partner either LOCKR or in page fault */
	andi	t1, s0, FS_LOCKS
	beq	t1, zero, 4f

	ld	t1, T_COMM_PARTNER(s2)
	lw	t2, T_FINE_STATE(t1)
	andi	a1, t2, FS_POLL
	beq	a1, zero, 5f /* check if partner POLL (rcv pf) */

	ld	t3, T_COMM_PARTNER(t1)
	rem_sendq(t1, t3, a2)
	
	/* assume partner is LOCKR and make busy */
5:	li	a2, L4_IPC_REABORTED
	make_busy(t1, a2)
	ins_busy_list(t1, a5, t0)
	sw	zero, T_STACKED_FINE_STATE(t1)
	li	a2, L4_IPC_SEABORTED
	make_busy(s2,a2)
	ins_busy_list(s2, a5, t0)
	b	1f
	
4:	/* case: LOCKR and partner either LOCKS or in page fault */
	andi	t1, s0, FS_LOCKR
	beq	t1, zero, 4f

	ld	t1, T_COMM_PARTNER(s2)
	lw	t2, T_FINE_STATE(t1)
	andi	a1, t2, FS_POLL
	beq	a1, zero, 5f /* check if partner POLL (rcv pf) */

	ld	t3, T_COMM_PARTNER(t1)
	rem_sendq(t1, t3, a2)
	
	/* assume partner is LOCKS and make busy */
5:	li	a2, L4_IPC_SEABORTED
	make_busy(t1, a2)
	ins_busy_list(t1, a5, t0)
	sw	zero, T_STACKED_FINE_STATE(t1)
	li	a2, L4_IPC_REABORTED
	make_busy(s2,a2)
	ins_busy_list(s2, a5, t0)
	b	1f
	

4:	/* case: LOCKS in pf, partner must be LOCKR */

	lw	t0, T_STACKED_FINE_STATE(s2)
	andi	t1, t0, FS_LOCKS
	beq	t1, zero, 4f

	ld	t1, T_STACKED_COMM_PRTNR(s2)
	lw	t2, T_FINE_STATE(t1)
	li	a2, L4_IPC_REABORTED
	make_busy(t1, a2)
	ins_busy_list(t1, a5, t0)
	sw	zero, T_STACKED_FINE_STATE(t1)
	andi	t0, s0, FS_POLL
	beq	zero, t0, 5f
	ld	t1, T_COMM_PARTNER(s2)
	rem_sendq(s2, t1, t2)
5:	li	a2, L4_IPC_SEABORTED
	make_busy(s2,a2)
	ins_busy_list(s2, a5, t0)
	b	1f

4:	/* case: LOCKR in pf, partner must be LOCKS */
	andi	t1, t0, FS_LOCKR
	beq	t1, zero, 4f

	ld	t1, T_STACKED_COMM_PRTNR(s2)
	lw	t2, T_FINE_STATE(t1)
	li	a2, L4_IPC_SEABORTED
	make_busy(t1, a2)
	ins_busy_list(t1, a5, t0)
	sw	zero, T_STACKED_FINE_STATE(t1)
	andi	t0, s0, FS_POLL
	beq	zero, t0, 5f
	ld	t1, T_COMM_PARTNER(s2)
	rem_sendq(s2, t1, t2)
5:	li	a2, L4_IPC_REABORTED
	make_busy(s2,a2)
	ins_busy_list(s2, a5, t0)
	b	1f


4:
	
	andi	t0, s0, FS_WAIT
	beq	zero, t0, 2f
	/* thread was waiting */
	dli	a0, L4_IPC_RECANCELED
	make_busy(s2, a0)
	ins_busy_list(s2, a5, t0)
	b	1f
2:
	andi	t0, s0, FS_POLL
	beq	zero, t0, 2f
	/* thread pending */
	ld	t1, T_COMM_PARTNER(s2)
	rem_sendq(s2, t1, t2)
	dli	a0, L4_IPC_SECANCELED
	make_busy(s2, a0)
	ins_busy_list(s2, a5, t0)
	b	1f
2:
	andi	t0, s0, FS_INACTIVE
	beq	zero, t0, 2f
	/* thread was waiting */
	beq	s8, zero, 1f   /* no change in sp so leave dead */
	move	a0, zero
	make_busy(s2, a0)
	ins_busy_list(s2, a5, t0)
	b	1f

2:
	andi	t0, s0, FS_BUSY
	bne	zero, t0, 1f
	
	dla	a0, msg_tcb_state
	j	panic /* ok */
	


1:	
	trace(exre)
	syscall_ret 	
END(k_lthread_ex_reg)

PROC(nchief)
	/* this routine uses 
	a0 = dthrd (conflict with ipc)
	t8 = stcb (same as ipc path)
	t2 = (no conflict with ipc path)
	t9 = dtcb (same as ipc)
	t0 = temp (no conflict)
	v1 = nearest (conflicts)
	v0 = type (no conflict)
	*/
	.set	noreorder
	tcbtop(t8)

	ld	t2, T_MYSELF-TCBO(t8)
	tid2tcb(a0, t9)
	ld	v1, T_MYSELF(t9)
long_ipc_nchief:
	.globl long_ipc_nchief
	
	/* first check if same chief */
	xor	t0, v1, t2
	dsll	t0, t0, 4
	dsrl	t0, t0, 53
	beq	t0, zero, same_clan

	/* now check if receiver is chief */
	dsll	t0, v1, 32
	xor	t0, t0, t2
	dsll	t0, t0, 4
	dsrl	t0, t0, 53
	beq	t0, zero, outer_clan


	/* transform sender into a chief tid */
	dsll	a7, t2, 32	
	
	/* t2 = sender */
	/* a7 = sender as chief */

	/* do until found */
	
1:
	/* check if sender is chief */
	xor	t0, v1, a7
	dsll	t0, t0, 4
	dsrl	t0, t0, 53
	beq	t0, zero, inner_clan /* sender is chief */
	
	/* check if same chief */
	xor	t0, v1, t2
	dsll	t0, t0, 4
	dsrl	t0, t0, 53
	beq	t0, zero, inner_clan /* same chief */
	
ipc_nchief:
	.globl ipc_nchief
	/* check if sender greater nesting than receiver */
	dsrl	t0, v1, 60
	dsrl	t1, t2, 60
	dsubu	t1, t0, t1
	blez	t1, outer_clan

	
	/* hmmm, move dest to dest->chief and try again */
	dli	t1,TID_TASK_MASK
	dsrl	t0, v1, 32
	and	t0, t1, t0
	tid2tcb(t0, t1)
	b	1b
	ld	v1, T_MYSELF(t1)
	
	
outer_clan:
	dli	t1,TID_TASK_MASK  
	dsrl	t0, t2, 32
	and	t0, t1, t0
	tid2tcb(t0, t1)
	dli	v0, L4_NC_OUTER_CLAN
	jr	ra
	ld	v1, T_MYSELF(t1)
		
inner_clan:
	jr	ra
	li	v0, L4_NC_INNER_CLAN
	
same_clan:
	jr	ra
	li	v0, L4_NC_SAME_CLAN
	.set reorder
END(nchief)

#ifdef MAP_INSTR
PROC(set_map_vec)
	/* a0 has the address */
	lui	t0, KERNEL_BASE

	/* use the TLB MISS counter as the vector, ugly I know but...  */
	sd	a0, K_TLB_MISS(t0)
	syscall_ret 
END(set_map_vec)	
#endif
	
