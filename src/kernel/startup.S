/****************************************************************************
 *      $Id: startup.S,v 1.3 1999/09/09 03:17:31 gernot Exp $
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
#include <regdef.h>
#ifdef GT_TICK
#include <kernel/gt64010a.h>
#endif	
#include <kernel/machine.h>
#include <r4kc0.h>
#include <asm.h>
#include <kernel/kernel.h>
#include <kernel/dit.h>	
#include <l4/syscalls.h>
#include <l4/sigma0.h>
#include "macros.h"

	.rdata
exec_set:
	.ascii "eset"
int_set:
	.asciiz"int set, waiting??"
msg_excpt:
	.asciiz "yippee!!! caught an exception."
dit_panic_msg:
	.asciiz "L4 PANIC: Can't find valid DIT header during startup"
no_asid_msg:
	.asciiz "no asid left in startup?"
lost_startup_msg:
	.asciiz "can't find startup thread in busy list 255"
here_msg:
	.ascii "L4_PANIC: Could not allocate stack for startup driver"
	
	.globl 	exc_jump_tbl

PROC(main)
	/* do data cache init */
	.set	noreorder
	dli	t0, 0xffffffff80000000
	li	t1, 255
	mtc0	zero, C0_TAGLO
2:	
	cache 0x9,0x0(t0)
	cache 0x9,0x2000(t0)
	lw	zero, (t0)
	lw	zero, 0x2000(t0)
	cache 0x9,0x0(t0)
	cache 0x9,0x2000(t0)
	addu	t0,t0,0x20	
	bgtz	t1, 2b
	addi	t1, t1, -1
	.set	reorder

	/* load the stack pointer for c */
	/* make it high in tcb space so we have plenty
	of stack to boot with (sizemem is memory hog) and initial
	tcb tlb refill needs a non-virtual stack to bootstrap */

	dli	sp, DEBUGGER_STACK

	lui	s0, KERNEL_BASE
	sd	sp, K_STACK_BOTTOM(s0)

#ifndef INDY
	/* init the 4 char display */
	jal	alphn_init
#endif
	
	/* init exception handling */
	trace(exci)
	jal	exc_init
	
	/* init error exception handling */
	trace(caci)
	jal	cache_err_init

	/* init syscalls */
	trace(sysi)
	jal	syscall_init

	/* init tcbs */
	trace(tcbi)
	jal	tcb_init

	/* get memsize */
	trace(smem)
	lui	s0, KERNEL_BASE
	sd	zero, K_MEMORY_SIZE(s0)

#ifdef U4600
	/* read the ram config from nvram */
	dli	t0, PHYS_TO_CKSEG1(RTCLOCK_BASE)
	lw	t1,  NVOFF_BANK0(t0)
	lw	t2,  NVOFF_BANK1(t0)
	daddu	v0, t1, t2
	lw	t1, NVOFF_BANK2(t0)	
	lw	t2, NVOFF_BANK3(t0)
	daddu	v0, v0, t1
	daddu	v0, v0, t2
	sd	v0, K_MEMORY_SIZE(s0)
#else
	/* HARD CODE 64M RAM */
	dli	v0, 64 * 1024 * 1024
	sd	v0, K_MEMORY_SIZE(s0)
#endif
	/* init asid allocator */
	jal	asid_init

	/* initialise frame table */
	trace(sigi)
	dla	a0, _end
	move    a1, v0
	jal	sigma0_init 
	/* vm  */
	trace(vmmi)
	jal	vm_mem_init

	trace(tbci)
#ifdef	TLB_CACHE
	jal	tlb_cache_init
#endif

	/* initialize invalid TCB */
	dli	s0, PHYS_TO_CKSEG1(INVALID_TCB_BASE)
	daddiu	s1, s0, TCB_SIZE  /* second thread tcb */
	li	s2, CS_INVALID_TCB
	sd	zero, T_MYSELF(s0)	
	sd	zero, T_MYSELF(s1)
	sw	s2, T_COARSE_STATE(s0)
	sw	s2, T_COARSE_STATE(s1)	

	/* set up first thread magically */

	/* first init kernel vars */
	
	lui	s0, KERNEL_BASE
	dli	s1, TCB_VBASE	

	
	/* K_STACK_BOTTOM done above */
	dli	t0, END_LIST

	sd	t0,  K_WAKEUP_LIST(s0)
	sd	zero, K_CLOCK(s0)
	sd	s1, K_PRESENT_LIST(s0)
	sd	t0, K_INT_LIST(s0)
	sd	t0,  K_SOON_WAKEUP_LIST(s0)
	sd	t0,  K_LATE_WAKEUP_LIST(s0)
	sd	zero, K_BREAK_ON(s0)
	sd	zero, K_INT0_THREAD(s0)
	sd	zero, K_INT1_THREAD(s0)
	sd	zero, K_INT2_THREAD(s0)
	sd	zero, K_INT3_THREAD(s0)
	sd	zero, K_INT4_THREAD(s0)
	/* enable floating point */
	sd	zero, K_FP_THREAD(s0)
	/* use for intrumentation */
#ifdef TLB_INSTR	
	sd	zero, K_TLB_MISS(s0)
	sd	zero, K_TLB_MISS_TIME(s0)
	sd	zero, K_TLB2_MISS(s0)
#elif MAP_INSTR
	sd	zero, K_TLB_MISS(s0)
#endif
	dli	t0, MAX_PRIORITY
	sw	t0, K_PRIORITY(s0)
	dli	t0, MAX_TIMESLICE
	sw	t0, K_TIMESLICE(s0)

#if 0 && defined(NOSGI)
	move	t0, zero
	lw	t0, K_TIMESLICE(s0)
	jal	dbg
#endif	

	/* initialise priority busy list */

	/* initially put spinner in MAX_PRIORITY list */ 
	daddiu	t0, s0, BUSY_LIST_SIZE
	sd	s1, K_PRIO_BUSY_LIST(t0) 
	
	dli	t0, 0
	dli	t1, BUSY_LIST_SIZE
ini_pbl:
	beq	t0, t1, ini_pbl_end
	daddu	t2, t0, s0 
	sd	zero, K_PRIO_BUSY_LIST(t2)
	daddiu	t0, t0, 8
	b	ini_pbl
ini_pbl_end:
	
#ifdef FLOAT64
	.set	noreorder
	mfc0	t0, C0_STATUS
	li	t1, ST_FR
	or	t0, t0, t1
	mtc0	t0, C0_STATUS
	.set	reorder
#endif	
	trace(vmia)
	jal	vm_initial_as

        /* set up virtual stack for thread 0 (the spinner) */
	dli	a0, TCB_SIZE-8
	daddu	sp, s1, a0      /* task 0 tcb */
	sd	sp, K_STACK_BOTTOM(s0)


	/* init tcb 0 vars */
	init_tcb(s1)
	
	
	sd	s1, T_BUSY_LINK(s1) /* put in busy list (never gets removed) */

	daddiu	t1, s1, TCB_SIZE /*we ensure second tcb in page invalid*/
	dli	t0, SIGMA0_TID  /* incorrect so to appear invalid */
	sd	t0, T_MYSELF(s1)
	sd	t0, T_MYSELF(t1)

	
	dli	t0, FS_BUSY
	sw	t0, T_FINE_STATE(s1)
	sw	zero, T_FINE_STATE(t1)

	dli	t0, CS_USED_TCB
	sw	t0, T_COARSE_STATE(s1)
	sw	t0, T_COARSE_STATE(t1)

	sd	zero, T_PRESENT_NEXT(s1)

	dli	t0, MAX_TIMESLICE
	sh	t0, T_TIMESLICE(s1)
	sh	t0, T_REM_TIMESLICE(s1)	
	/*sh	t0, T_TIMESLICE(t1)
	sh	t0, T_REM_TIMESLICE(t1)*/

	/* set to max to enable initial servers to inherit max priority;
	   needs to be reset after starting intial servers */ 
	dli	t0, MAX_PRIORITY
	sb	t0, T_MCP(s1)
	sb	t0, T_TSP(s1)
	sb	t0, T_CTSP(s1)
	/*sb	t0, T_MCP(t1)
	sb	t0, T_CTSP(t1)*/

	/* mark tcb as valid in tcb table */
	tid2ttable(zero,t0)
	sw	zero, (t0)

	ld	s2, T_GPT_POINTER(s1)
/****************************************************************************
 * initialise interrupt sending tcbs
 ****************************************************************************/

	trace(iitb)
	dli	s5, INT0_TCB_BASE & (~(TCB_SIZE -1))
	init_tcb(s5)
	dli	s4, 1
	sd	s4, T_MYSELF(s5)
	sd	s2, T_GPT_POINTER(s5)
	li	t0, CS_USED_TCB
	sw	t0, T_COARSE_STATE(s5)
	
	dli	s5, INT1_TCB_BASE & (~(TCB_SIZE -1))
	init_tcb(s5)
	dli	s4, 2
	sd	s4, T_MYSELF(s5)
	sd	s2, T_GPT_POINTER(s5)
	sw	t0, T_COARSE_STATE(s5)
	
	dli	s5, INT2_TCB_BASE & (~(TCB_SIZE -1))
	init_tcb(s5)
	dli	s4, 3
	sd	s4, T_MYSELF(s5)
	sd	s2, T_GPT_POINTER(s5)
	sw	t0, T_COARSE_STATE(s5)

	dli	s5, INT3_TCB_BASE & (~(TCB_SIZE -1))
	init_tcb(s5)
	dli	s4, 4
	sd	s4, T_MYSELF(s5)
	sd	s2, T_GPT_POINTER(s5)
	sw	t0, T_COARSE_STATE(s5)
	
	dli	s5, INT4_TCB_BASE & (~(TCB_SIZE -1))
	init_tcb(s5)
	dli	s4, 5
	sd	s4, T_MYSELF(s5)
	sd	s2, T_GPT_POINTER(s5)
	sw	t0, T_COARSE_STATE(s5)
	/*sb	t1, T_MCP(s5)
	sb	t1, T_CTSP(s5)
	sh	t2, T_TIMESLICE(s5)
	sh	t2, T_REM_TIMESLICE(s5)	*/
	
/****************************************************************************
 * start sigma0 as a kernel thread 
 ****************************************************************************/
	trace(sg0i)
	/* get vaddress of tcb */
	dli	s4, SIGMA0_TID
	tid2tcb(s4, s5)
	lui	t0, KERNEL_BASE
	sd	s5, K_SIGZ_TCB(t0)
	daddiu	s6, s5, TCB_SIZE
	daddiu	s7, s6, TCB_SIZE -16
	/* mark valid in ttable */
	tid2ttable(s4, t0)
	sw	zero, (t0)

	/* add tcb mapping in shared tree */
	move	a0, s2   /* gpt pointer */
	move	a1, s5   /* virtual tcb base */
	dli	a2, CKSEG0_TO_PHYS(SIGMA0_TCB_BASE)
	trace(vmti)
	jal	vm_tcb_insert
	trace(vtme)
	daddiu	s1, s5, TCB_SIZE -16  /* s1 now contains top of new stack */
	dla	s0, sigma0_thread

	/* now build a stack to switch to */
	dla	t1, sigma0_excpt_thread
	sd	s1, -8(s1)	/* new thread sp */
	sd	s7, -8(s7)
	sd	s0, -16(s1)	/* new thread start address */
	sd	t1, -16(s7)
	li	t1, (ST_KX|ST_UX|ST_SX|ST_EXL|ST_IE ) /* status */
	sb	t1, -24(s1)
	sb	t1, -24(s7)
	dla	t0, sigma_excpt_restart
	dla	t1, sigma_zero_restart
	sd	t1, -32(s1)
	sd	t0, -32(s7)
	daddiu	s1,s1,-32
	daddiu	s7,s7,-32

	/* initialise most tcb vars */

	init_tcb(s5)
	init_tcb(s6)
	sd	s4, T_MYSELF(s5)
	daddiu	t0, s4, 1 << 10
	sd	t0,  T_MYSELF(s6)

	/* init the page table */
	move	a0, s5
	trace(newa)
	jal	vm_new_as

	ld	v0, T_GPT_POINTER(s5)
	sd	v0, T_GPT_POINTER(s6)

	sd	zero, T_PAGER_TID(s5)
	sd	zero, T_PAGER_TID(s6)

	trace(asid)
	/* allocate an asid */
	move	a0, s4
	jal	asid_alloc  /* uses t0, v0, AT, ra */
	sd	v0, T_ASID(s5)
	sd	v0, T_ASID(s6)

	/* add new thread to run queue */
	sd	s1, T_STACK_POINTER(s5)
	sd	s7, T_STACK_POINTER(s6)
	
	/* set scheduling variables */
	dli	t0, MAX_TIMESLICE
	sh	t0, T_REM_TIMESLICE(s5)
	sh	t0, T_TIMESLICE(s5)
	sh	t0, T_REM_TIMESLICE(s6)
	sh	t0, T_TIMESLICE(s6)		

	dli	t0, MAX_PRIORITY
	sb	t0, T_MCP(s5)
	sb	t0, T_CTSP(s5)
	sb	t0, T_TSP(s5)
	sb	t0, T_MCP(s6)
	sb	t0, T_CTSP(s6)
	sb	t0, T_TSP(s6)

	trace(busy)
	lui	a2, KERNEL_BASE
	ins_busy_list(s5, a2, t0)
	ins_busy_list(s6, a2, t0)

	/* have null length present list */
	sd	s6, T_PRESENT_NEXT(s5)
	sd	zero, T_PRESENT_NEXT(s6)
	/* set running state */
	li	t2, FS_BUSY
	sw	t2, T_FINE_STATE(s5)
	sw	t2, T_FINE_STATE(s6)

	.set noreorder
#ifdef P4000
	dli	a1, PHYS_TO_CKSEG1(IPANIC) /* clear previous ints if any */
	lw      zero, (a1)
#endif


#ifdef PROFILE
	dli	t0, PROFILE_START_ADDR
	lui	a0, KERNEL_BASE
	sd	t0, K_PROFILE_ADDR(a0)
#endif

	/* set up timer interrupts */
#ifdef GT_TICK
#define bswap(x) ((((x) & 0x000000FF) << 24) | \
	  (((x) & 0x0000FF00) << 8)  | \
	  (((x) & 0x00FF0000) >> 8)  | \
	  (((x) & 0xFF000000) >> 24))
	
	dli	t0, PHYS_TO_CKSEG1(GT_BASE_ADDR+GT_TIMER_CNTRL)
	sw	zero, (t0)
	sync
	dli	t1, PHYS_TO_CKSEG1(GT_BASE_ADDR+GT_INT_CAUSE)
	li	a0, bswap(~GT_INT_TIM0EXP)
	sw	a0,(t1)
	sync
	dli	t1, PHYS_TO_CKSEG1(GT_BASE_ADDR+GT_INT_CPU_MASK)
	li	a0, bswap(GT_INT_TIM0EXP)
	sw	a0, (t1)
	sync
	dli	t1, PHYS_TO_CKSEG1(GT_BASE_ADDR+GT_TIMER0)
	li	a0, bswap(50000)
	sw	a0, (t1)
	sync
	li	a0, bswap(GT_TIM0_EN | GT_TIM0_TMR)
	sw	a0, (t0)
	sync
#else	
	li	t0, INITIAL_TIMER_TICKS /* clear interupt and counter */
	mtc0	t0, C0_COMPARE
	mtc0    zero, C0_COUNT
	nop
#endif
	trace(stat)		
	mfc0	a0, C0_STATUS
#ifdef INDY
	li	a1, (ST_IM7| ST_IM4 | ST_IE)
	or	a0, a0, a1
	li	a1, ~(ST_DE | ST_IM6 | ST_IM5 | ST_IM3 | ST_IM2 )
	and	a0, a0, a1
#else
#ifdef GT_TICK	
	li	a1, (ST_IM5|  ST_IM6 | ST_IE)
#else
	li	a1, (ST_IM7|  ST_IM6 | ST_IE)
#endif	
	or	a0, a0, a1
	li	a1, ~ST_DE
	and	a0, a0, a1
#endif
	mtc0	a0, C0_STATUS
	nop
	.set reorder
	
	/* okay we should be able to catch the debug switch now */


/****************************************************************************
 *
 * Now sigma0 zero exists, we "create task" each of the programs downloaded
 * with the kernel image.
 *
 ****************************************************************************/
	trace(crti)
	/* move to dit by rounding up _end */
	dla	s0, _end
	ori	s0, L4_PAGESIZE -1
	daddiu	s0, s0, 1

	/* check if header exist */
	lbu	t0, D_D_IDENT(s0)
	dli	t1, 0144
	bne	t0, t1, 2f
	lbu	t0, D_D_IDENT+1(s0)
	dli	t1, 0150
	bne	t0, t1, 2f
	lbu	t0, D_D_IDENT+2(s0)
	dli	t1, 0144
	bne	t0, t1, 2f
	lbu	t0, D_D_IDENT+3(s0)
	dli	t1, 0162
	bne	t0, t1, 2f

	/* load various values */
	lw	s1, D_D_PHOFF(s0)
	daddu	s1, s1, s0
	
	lw	s2, D_D_PHNUM(s0)
	lw	s4, D_D_PHSIZE(s0)
	dli	s5, 3

	/* loop through creating each task */
3:	beq	s2, zero, 1f
	lw	t0, D_P_FLAGS(s1)
	andi	t0, t0, DIT_RUN
	beq	t0, zero, 4f

	dsll	a0, s5, 17
	dli	a1, MAX_PRIORITY /* to allow inheritence of creator mcp */
	dli	a2, 0 /* stack set up in crtS.o in initial tasks*/
	lw	a3, D_P_ENTRY(s1)
	dli	a4, SIGMA0_TID
	dli	a5, SIGMA0_TID+ (1 << 10)

	jal	l4_task_new

4:	daddu	s1, s1, s4
	daddiu	s2,s2,-1
	daddiu	s5, s5, 1
	.set	noat
	b	3b

2:	
	dla	a0, dit_panic_msg
	j	panic /* ok */
3:	dla	a0, here_msg
	j	panic /* ok */

1:	/* reset spinning thread's (i.e. current thread's) priority to 0 */
	dli	s3, TCB_VBASE
	sb	zero, T_MCP(s3)
	sb	zero, T_TSP(s3)
	sb	zero, T_CTSP(s3)	

	/* remove spinner from list255 */
	lui	t1, KERNEL_BASE
	daddiu	t1, t1, BUSY_LIST_SIZE /* t1 contains offset to list255 */
	ld	t2, K_PRIO_BUSY_LIST(t1)
	beq	zero, t2, rs_err1 /* list255 is empty */
	ld	t3, T_BUSY_LINK(t2) /* t3 is next tcb pointer */
	move	t0, t2 /* t0 is prev tcb pointer */
		
rs_loop:	
	beq	t3, s3, rs_found /* found spinner - remove it now */
	beq	t3, t2, rs_err1 /* spinner not in list255 */
	move	t0, t3 /* prev = next */
	ld	t3, T_BUSY_LINK(t3) /* next = next->next */	
	b	rs_loop
rs_found:
	beq	t0, s3, rs_only  /* check if queue only contains spinner */
	ld	t2, K_PRIO_BUSY_LIST(t1)
	bne	t2, s3, rs_rem	/* check if spinner is at tail of list255 */
	sd	t0, K_PRIO_BUSY_LIST(t1)
rs_rem:
	ld	t2, T_BUSY_LINK(t3)
	sd	t2, T_BUSY_LINK(t0)
	b	rs_end
rs_only:
	/* list255 only contains spinner */
	sd	zero, K_PRIO_BUSY_LIST(t1)
rs_end:
	sd	zero, T_BUSY_LINK(s3)
	lui	t0, KERNEL_BASE
	ins_busy_list(s3, t0, t1)
	b	1f
rs_err1:
	dla	a0, lost_startup_msg
	j	panic	/* ok */
	
1:

/****************************************************************************
 *
 * from here on, this thread is left in runq to call thread_switch
 *
 ****************************************************************************/
	.set	noat
	.set	noreorder
	
	dli	a0, 0
	dli	AT, SYSCALL_THREAD_SWITCH
	syscall
	nop
	b	1b
	nop
	.set	reorder
	.set at
END(main)

PROC(sigma_zero_restart)
	daddiu	sp, sp, 8
	syscall_ret 
END(sigma_zero_restart)

PROC(sigma_excpt_restart)
	daddiu	sp, sp, 8
	syscall_ret 
END(sigma_excpt_restart)


.data
msg:
	.asciiz "before"
msg2:
	.asciiz "after"


