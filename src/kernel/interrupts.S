/****************************************************************************
 *      $Id: interrupts.S,v 1.3 1999/09/09 03:17:31 gernot Exp $
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
#include <l4/ipc.h>
#include <l4/sigma0.h>
#include <r4kc0.h>
#include <kernel/machine.h>
#include <kernel/panic.h>
#include <kernel/kernel.h>
#ifdef GT_TICK
#include <kernel/gt64010a.h>
#endif
#include "macros.h"

	.rdata
debug:	
	.asciiz	"debg"
tmr_msg:
	.asciiz "L4 PANIC: timer diverged"
int_msg:
	.asciiz "interrupt"
timeslice_err:
	.asciiz "L4 PANIC:kernel timeslice less than zero"

	
/* interrupt handlers must preserve s0 and return via jr ra */
#ifdef GT_TICK
#define bswap(x) ((((x) & 0x000000FF) << 24) | \
	  (((x) & 0x0000FF00) << 8)  | \
	  (((x) & 0x00FF0000) >> 8)  | \
	  (((x) & 0xFF000000) >> 24))
	
PROC(int_ip5)
	dla	t0, PHYS_TO_CKSEG1(GT_BASE_ADDR+GT_INT_CAUSE)
	li	t1, bswap(~GT_INT_TIM0EXP)
	sw	t1, (t0)
	sync
#else
				
PROC(int_ip7)
	.set noreorder
	
	mfc0	t1, C0_COUNT
	nop
	mtc0    zero, C0_COUNT
	li	t0, TIMER_TICKS /* clear interrupt and counter */
	subu	t0, t0, t1
	mfc0	t1, C0_COMPARE
	bltz	t0, 1f
	nop
	b	2f
	addiu	t1, t1, 1

1:	addiu	t1, t1, -1
	
2:	mtc0	t1, C0_COMPARE
#endif	
	lui	t1, KERNEL_BASE
	ld	t0, K_CLOCK(t1)
	daddiu	t0, t0, 1
	sd	t0, K_CLOCK(t1)
#ifdef NO_CACHING
        dli     t2, PHYS_TO_CKSEG1(KERNEL_INFO_PAGE)
#else /* !NO_CACHING */
	dli	t2, PHYS_TO_CKSEG0(KERNEL_INFO_PAGE)
#endif /* !NO_CACHING */
	sd	t0, LKI_CLOCK(t2)
	move	s1, ra

#ifdef PROFILE	
	ld	t2, K_PROFILE_ADDR(t1)
	dli	t3, PROFILE_END_ADDR
	daddiu	t2, t2, 8
	bne	t2, t3, 1f
	nop
	dli	t2, PROFILE_START_ADDR
1:	tcbtop(t3)
	sd	t2, K_PROFILE_ADDR(t1)
	ld	t3, ST_EPC+1(t3)
	sd	t3, (t2)
#endif 
	/* decrement kernel remaining timeslice */
	lui	a0, KERNEL_BASE
	lw	t9, K_TIMESLICE(a0)

	daddiu	t9, t9, -1
	bltz	t9, ip7_err
	nop
	sw	t9, K_TIMESLICE(a0)

	jal	check_wake_up
	nop
	
	/* okay old tcb is s3, new tcb is v0 */
	tcbtop(s2)
	daddiu	s3,s2,-TCBO
	
	/* first increment old thread's cpu time */
	ld	t9, T_CPU_TIME(s3)
	daddiu	t9, t9, 1000
	sd	t9, T_CPU_TIME(s3)

	beq	v0, zero, 1f /* no need to reschedule on this tick */
	nop

	lui	a0, KERNEL_BASE
	lw	t2, K_PRIORITY(a0)
	lw	t3, K_TIMESLICE(a0)

	/* load new kernel scheduling values */
	lbu	t0, T_CTSP(v0)
	sw	t0, K_PRIORITY(a0)
	lhu	t0, T_REM_TIMESLICE(v0)
	sw	t0, K_TIMESLICE(a0)

	/* check if new thread is a wakeup */
	lw	t0, T_FINE_STATE(v0)
	andi	t0, t0, FS_WAKEUP
	beq	zero, t0, ip7_tswitch1 /* new thread is not a wakeup */
	nop
	dli	t0, FS_BUSY
	sw	t0, T_FINE_STATE(v0)
	ins_busy_list(v0, a0, t0)
	
	/* stack the thread to be preempted */
	beq	zero, t3, ip7_tswitch1 /* no time left - don't stack */
	nop
	sh	t3, T_REM_TIMESLICE(s3)
	sb	t2, T_CTSP(s3)
	ins_int_list(s3, a0)
	b	ip7_tswitch2
	nop

	/* thread switch */
ip7_tswitch1:
	/* make sure thread we are preempting is in busy list */
	ins_busy_list(s3, a0, t0) 
ip7_tswitch2:
	beq	v0, s3, 1f /* old and new are the same - no thread switch */
	nop
	daddiu	sp,sp,-8
	dla	t0, preempt_ret
	sd	t0, (sp)

	thread_switch_fast(s2,v0,a0)
	.set noreorder
	ld	ra, (sp)
	jr	ra
	nop

	/* FIXME: interrupts aren't handled fairly */
	
1:
	jr	s1
	nop
	.set reorder

ip7_err:
	dla	a0, timeslice_err
	j	panic	/* ok */
#ifdef GT_TICK
END(int_ip5)
PROC(int_ip7)
END(int_ip7)		
#else		
END(int_ip7)
#endif
PROC(preempt_ret)
	daddiu	sp, sp, 8
	j	other_excpt_ret
END(preempt_ret)

PROC(int_ip6)
	/* debug switch */

#ifdef INDY
	dla	a0, int_msg
	move	s1, ra
	jal	dbg
	jr	s1
#endif
#ifdef P4000
	/* debug switch */
	dla	a0, int_msg
	move	s1, ra
	jal	dbg
	dli	a0, PHYS_TO_CKSEG1(IPANIC) /* clear previous ints */
	lw      zero, (a0)
	jr	s1
#endif

#ifdef U4600 	
	dla	a0, int_msg
	move	s1, ra
	jal	dbg
	jr	s1
#endif

sdbg:
#if 0
	trace(int4)
	/* first disassociate from current thread */
	tcbtop(t8)
	daddiu	s1,t8,-TCBO
	daddiu	sp,sp,-8
	dla	t0, preempt_ret
	sd	t0, (sp)
	lui	s2, KERNEL_BASE
	/* make busy ?? should already be busy */
	ins_busy_list(s1, s2, t0)
	sd	sp, T_STACK_POINTER-TCBO(t8)

	/* mask interrupt */
	.set	noreorder
	mfc0	t1, C0_STATUS
	li	t2, ~(ST_IM6)
	and	t2, t1, t2
	mtc0	t2, C0_STATUS

	/* switch to sending thread */
	dmtc0	zero, C0_ENTRYHI
	.set	reorder
	dli	sp, INT4_TCB_BASE
	tcbtop(t8)
	ld	t0, T_GPT_POINTER-TCBO(t8)
	sd	t0, K_GPT_POINTER(s2)

	/* send ipc */
	move	t0, sp
	daddiu	sp, sp, -24    /* fake a stack such that syscall_ret actually 
				does returns here */	
	sb	t2, (sp)
	dla	t3, 1f
	sd	t3, 8(sp)
	sd	t0, 16(sp)
	
	
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_NIL_DESCRIPTOR
	dli	a2, L4_IPC_NEVER	
	ld	a4, K_INT4_THREAD(s2)

	/* now fake start of ipc routine */	
	/* stcb (t8) has tcb top */
	tid2tcb(a4,t9)
	ld	t0, T_MYSELF(t9)
	dli	v1, 5
	lw	t3, T_FINE_STATE(t9)
	move	v0, zero

	j	return_to_chief1

1:	tcbtop(t0)
	lui	t1, KERNEL_BASE
	andi	v0, v0, L4_IPC_ERROR_MASK
	beq	v0, zero, 2f

	/* interrupt send failed, so dissassociate from thread */
	
	sd	zero, 	K_INT4_THREAD(t1)
	sd	zero, T_INTERRUPT_MASK-TCBO(t0)

2:	sw	zero, T_FINE_STATE-TCBO(t0)
	jal	dbg
	to_next_thread(t1)
#endif	
END(int_ip6)

#ifndef GT_TICK
PROC(int_ip5)
	trace(int3)
	/* first disassociate from current thread */
	tcbtop(t8)
	daddiu	s1,t8,-TCBO
	daddiu	sp,sp,-8
	dla	t0, preempt_ret
	sd	t0, (sp)
	lui	s2, KERNEL_BASE
	sd	sp, T_STACK_POINTER-TCBO(t8)

	/* mask interrupt */
	.set	noreorder
	mfc0	t1, C0_STATUS
	li	t2, ~(ST_IM5)
	and	t2, t1, t2
	mtc0	t2, C0_STATUS

	/* stack interrupted thread */
	dli	t0, INT3_TCB_BASE & (~(TCB_SIZE - 1))
	int_preempt(s1, t0, s2, t3)	
	
	/* switch to sending thread */
	dmtc0	zero, C0_ENTRYHI
	.set	reorder
	dli	sp, INT3_TCB_BASE
	tcbtop(t8)
	ld	t0, T_GPT_POINTER-TCBO(t8)
	sd	t0, K_GPT_POINTER(s2)

	/* send ipc */
	move	t0, sp
	daddiu	sp, sp, -24    /* fake a stack such that syscall_ret actually 
				does returns here */	
	sb	t2, (sp)
	dla	t3, 1f
	sd	t3, 8(sp)
	sd	t0, 16(sp)
	
	
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_NIL_DESCRIPTOR
	dli	a2, L4_IPC_NEVER	
	ld	a4, K_INT3_THREAD(s2)

	/* now fake start of ipc routine */	
	/* stcb (t8) has tcb top */
	tid2tcb(a4,t9)
	ld	t0, T_MYSELF(t9)
	dli	v1, 4
	lw	t3, T_FINE_STATE(t9)
	move	v0, zero

	j	return_to_chief1

1:	tcbtop(t0)
	lui	t1, KERNEL_BASE
	andi	v0, v0, L4_IPC_ERROR_MASK
	beq	v0, zero, 2f

	/* interrupt send failed, so dissassociate from thread */
	
	sd	zero, K_INT3_THREAD(t1)
	sd	zero, T_INTERRUPT_MASK-TCBO(t0)

2:	sw	zero, T_FINE_STATE-TCBO(t0)
	to_next_thread(t1)	
END(int_ip5)
#endif
	
PROC(int_ip4)
#ifdef INDY
	dla	a0, int_msg
	move	s1, ra
	jal	dbg
	jr	s1
#else
	trace(int2)
	tcbtop(t8)
	daddiu	s1,t8,-TCBO
	daddiu	sp,sp,-8
	dla	t0, preempt_ret
	sd	t0, (sp)
	lui	s2, KERNEL_BASE
	sd	sp, T_STACK_POINTER-TCBO(t8)

	/* mask interrupt */
	.set	noreorder
	mfc0	t1, C0_STATUS
	li	t2, ~(ST_IM4)
	and	t2, t1, t2
	mtc0	t2, C0_STATUS

	/* stack interrupted thread */
	dli	t0, INT2_TCB_BASE & (~(TCB_SIZE - 1))
	int_preempt(s1, t0, s2, t3)
	
	/* switch to sending thread */
	dmtc0	zero, C0_ENTRYHI
	.set	reorder
	dli	sp, INT2_TCB_BASE
	tcbtop(t8)
	ld	t0, T_GPT_POINTER-TCBO(t8)
	sd	t0, K_GPT_POINTER(s2)

	/* send ipc */
	move	t0, sp
	daddiu	sp, sp, -24    /* fake a stack such that syscall_ret actually 
				does returns here */	
	sb	t2, (sp)
	dla	t3, 1f
	sd	t3, 8(sp)
	sd	t0, 16(sp)
	
	
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_NIL_DESCRIPTOR
	dli	a2, L4_IPC_NEVER	
	ld	a4, K_INT2_THREAD(s2)

	/* now fake start of ipc routine */	
	/* stcb (t8) has tcb top */
	tid2tcb(a4,t9)
	ld	t0, T_MYSELF(t9)
	dli	v1, 3
	lw	t3, T_FINE_STATE(t9)
	move	v0, zero

	j	return_to_chief1

1:	
	trace(it2r)
	tcbtop(t0)
	lui	t1, KERNEL_BASE
	andi	v0, v0, L4_IPC_ERROR_MASK
	beq	v0, zero, 2f

	/* interrupt send failed, so dissassociate from thread */
	
	sd	zero, K_INT2_THREAD(t1)
	sd	zero, T_INTERRUPT_MASK-TCBO(t0)

2:	sw	zero, T_FINE_STATE-TCBO(t0)
	to_next_thread(t1)	
#endif
END(int_ip4)

PROC(int_ip3)
	trace(int1)
	tcbtop(t8)
	daddiu	s1,t8,-TCBO
	daddiu	sp,sp,-8
	dla	t0, preempt_ret
	sd	t0, (sp)
	lui	s2, KERNEL_BASE
	sd	sp, T_STACK_POINTER-TCBO(t8)

	/* mask interrupt */
	.set	noreorder
	mfc0	t1, C0_STATUS
	li	t2, ~(ST_IM3)
	and	t2, t1, t2
	mtc0	t2, C0_STATUS
	
	/* stack interrupted thread */
	dli	t0, INT1_TCB_BASE & (~(TCB_SIZE - 1))
	int_preempt(s1, t0, s2, t3)

	/* switch to sending thread */
	dmtc0	zero, C0_ENTRYHI
	.set	reorder
	dli	sp, INT1_TCB_BASE
	tcbtop(t8)
	ld	t0, T_GPT_POINTER-TCBO(t8)
	sd	t0, K_GPT_POINTER(s2)

	/* send ipc */
	move	t0, sp
	daddiu	sp, sp, -24    /* fake a stack such that syscall_ret actually 
				does returns here */	
	sb	t2, (sp)
	dla	t3, 1f
	sd	t3, 8(sp)
	sd	t0, 16(sp)
	
	
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_NIL_DESCRIPTOR
	dli	a2, L4_IPC_NEVER	
	ld	a4, K_INT1_THREAD(s2)

	/* now fake start of ipc routine */	
	/* stcb (t8) has tcb top */
	tid2tcb(a4,t9)
	ld	t0, T_MYSELF(t9)
	dli	v1, 2
	lw	t3, T_FINE_STATE(t9)
	move	v0, zero

	j	return_to_chief1

1:	tcbtop(t0)
	lui	t1, KERNEL_BASE
	andi	v0, v0, L4_IPC_ERROR_MASK
	beq	v0, zero, 2f

	/* interrupt send failed, so dissassociate from thread */
	
	sd	zero, K_INT1_THREAD(t1)
	sd	zero, T_INTERRUPT_MASK-TCBO(t0)

2:	sw	zero, T_FINE_STATE-TCBO(t0)
	to_next_thread(t1)	
END(int_ip3)

PROC(int_ip2)
	trace(int0)
	tcbtop(t8)
	daddiu	s1,t8,-TCBO
	daddiu	sp,sp,-8
	dla	t0, preempt_ret
	sd	t0, (sp)
	lui	s2, KERNEL_BASE
	sd	sp, T_STACK_POINTER-TCBO(t8)

	/* mask interrupt */
	.set	noreorder
	mfc0	t1, C0_STATUS
	li	t2, ~(ST_IM2)
	and	t2, t1, t2
	mtc0	t2, C0_STATUS
	
	/* stack interrupted thread */
	dli	t0, INT0_TCB_BASE & (~(TCB_SIZE - 1))
	int_preempt(s1, t0, s2, t3)

	/* switch to sending thread */
	dmtc0	zero, C0_ENTRYHI
	.set	reorder
	dli	sp, INT0_TCB_BASE
	tcbtop(t8)
	ld	t0, T_GPT_POINTER-TCBO(t8)
	sd	t0, K_GPT_POINTER(s2)

	/* send ipc */
	move	t0, sp
	daddiu	sp, sp, -24    /* fake a stack such that syscall_ret actually 
				does returns here */	
	sb	t2, (sp)
	dla	t3, 1f
	sd	t3, 8(sp)
	sd	t0, 16(sp)
	
	
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_NIL_DESCRIPTOR
	dli	a2, L4_IPC_NEVER	
	ld	a4, K_INT0_THREAD(s2)

	/* now fake start of ipc routine */	
	/* stcb (t8) has tcb top */
	tid2tcb(a4,t9)
	ld	t0, T_MYSELF(t9)
	dli	v1, 1
	lw	t3, T_FINE_STATE(t9)
	move	v0, zero

	j	return_to_chief1

1:	tcbtop(t0)
	lui	t1, KERNEL_BASE
	andi	v0, v0, L4_IPC_ERROR_MASK
	beq	v0, zero, 2f

	/* interrupt send failed, so dissassociate from thread */
	
	sd	zero, K_INT0_THREAD(t1)
	sd	zero, T_INTERRUPT_MASK-TCBO(t0)

2:	sw	zero, T_FINE_STATE-TCBO(t0)
	to_next_thread(t1)	
END(int_ip2)

PROC(int_ip1)
	dla	a0, int_msg
	move	s1, ra
	jal	dbg
	jr	s1
END(int_ip1)

PROC(int_ip0)
	dla	a0, int_msg
	move	s1, ra
	jal	dbg
	jr	s1
END(int_ip0)

