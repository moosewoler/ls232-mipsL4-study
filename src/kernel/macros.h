/****************************************************************************
 *      $Id: macros.h,v 1.3 1999/10/28 04:00:12 alanau Exp $
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

/****************************************************************************
 * wakeup_remaining(tcb, time, temp)
 * 
 * calculate time remaining before wakeup
 * tcb: contains the wakeup time
 * time: wakeup time remaining in timeout format (m.s. 12 bits)
 * temp: temporary register
 ****************************************************************************/

#define wakeup_remaining(tcb, time, temp) \
        .set noat ; \
        ld      time, T_WAKEUP(tcb) ; \
        bne     time, zero, 1f ; \
        move    time, zero /* infinite wakeup left - i.e. no wakeup */ ; \
        b       2f ; \
1:      lui     AT, KERNEL_BASE ; \
        ld      AT, K_CLOCK(AT) ; \
        dsubu   time, time, AT ; \
        .set    at ; \
        dli     temp, 1000 ; \
        ddiv    time, temp ; \
        mflo    time ; \
        encode_time(time, AT, temp) ; \
        .set noat ; \
        dsll    AT, CT_TIME_MAN_SHIFT ; \
        dsll    temp, CT_TIME_EXP_SHIFT ; \
        or      time, time, AT ; \
        or      time, time, temp ; \
2:      .set at

/****************************************************************************
 * decode_time(m, e, output)
 * 
 * decode a timeout/timeslice format specification into a *millisecond* value 
 * m: register containing the 'm' value of a timeout/timeslice 
 * e: register containing the 'e' value of a timeout/timeslice
 * ouput: millisecond value returned in this register 
 ****************************************************************************/

#define decode_time(mant, exp, output) \
        /* check if m is zero */ ; \
        bne     zero, mant, 254f ; \
        move    output, zero /* zero time */ ; \
        b       255f ; \
254: ; \
        dli     output, 15 ; \
        sub     exp, output, exp ; \
        sll     exp, exp, 1  ; \
        sllv    output, mant, exp ; \
         ; \
        .set noreorder ; \
         ; \
        /* round to nearest millisecond */ ; \
        dli     mant, 1000 ; \
        ddiv    output, mant ; \
        dli     exp, 500 ; \
        nop ; \
        mflo    output ; \
        mfhi    mant ; \
        sub     mant, mant, exp ; \
        blez    mant, 255f /* round down */ ; \
        nop ; \
        addi    output, output, 1 /* round up */ ; \
 ; \
255: ; \
        .set reorder

/****************************************************************************
 * encode_time(input, m, e)
 * 
 * encode a *millisecond* value into a timeout/timeslice format 
 * input: millisecond value to be decoded; returns microsecond value   
 * m: register containing the 'm' value of the decoded timeout/timeslice 
 * e: register containing the 'e' value of the decoded timeout/timeslice
 ****************************************************************************/

#define encode_time(input, mant, exp) \
        .set noreorder ; \
        .set noat ; \
; \
        /* convert into microseconds */ ; \
        dli     AT, 1000 ; \
        dmult   input, AT ; \
        dli     mant, 0 ; \
        dli     exp, 1 /* any non-zero value is ok */ ; \
        mflo    input ; \
 ; \
        /* check if input is zero */ ; \
        bne     input, zero, 252f ; \
        nop ; \
        b       255f ; \
 ; \
252:    dli     AT, 28 /* e < 15 for millisecond precision */ ; \
 ; \
253:    dli     exp, 30 ; \
        dsubu   exp, exp, AT /* exp = 30 - AT */ ; \
        dsrlv   mant, input, exp /* mant = input DIV 2^(30 -AT) */ ; \
        dli     exp, 255 ; \
        dsubu   exp, exp, mant ; \
        bgez    exp, 254f ; \
        nop ; \
        daddiu  AT, AT, -2 ; \
        b       253b ; \
254:    move    exp, AT ; \
        dsrl    exp, 1 ; \
        .set at ; \
        .set reorder ; \
255:

/****************************************************************************
 *
 * ins_dl(input, list , offset)
 *
 * insert tcb into double linked list before head of list
 * input: register pointing to tcb to be linked in
 * list: register pointing to tcb already in list
 * offset: offset from tcb base containing next pointer (prev pointer must
 *      follow next)
 *
 * No null pointers in next/prev allowed
 *
 ****************************************************************************/

#define ins_dl(input, list, offset) \
        .set    noat ; \
        sd      list, offset(input) ; \
        ld      AT, offset+8(list) ; \
        sd      AT, offset+8(input) ; \
        sd      input, offset(AT) ; \
        sd      input, offset+8(list) ; \
        .set    at

/****************************************************************************
 *
 * tcbtop(tcb)
 *
 * derive top of tcb from stack pointer
 *
 ****************************************************************************/

#define  tcbtop(tcb) \
        ori     tcb, sp, TCBO
        
/****************************************************************************
 *
 * tid2tcb(tid, tcb)
 *
 ****************************************************************************/

#define tid2tcb(tid, tcb) \
        .set    noat  ; \
        dsll    tcb, tid, 36 ; \
        dsrl    tcb, tcb, 46 ; \
        dsll    tcb, tcb, TCB_SIZE_SHIFT ; \
        dli     AT, TCB_VBASE ; \
        daddu   tcb, tcb, AT ; \
        .set    at


/****************************************************************************
 *
 * tid2ttable(tid, address)
 *
 ****************************************************************************/

#define tid2ttable(tid, address) \
	.set	noat ; \
	dsrl	address, tid, 17 ; \
	dli	AT, 03777 ; \
	and	address, address, AT ; \
	dsll	address, address, 2 ; \
	dli	AT, TID_TABLE_BASE ; \
	daddu	address, address, AT

/****************************************************************************
 *
 * thread_switch_fast(stcb, dtcb, kernel_base) 
 *
 * stcb: top of sender tcb
 * dtcb: bottom of destination tcb
 * kernel base
 *
 ****************************************************************************/
	
#define  thread_switch_fast(stcb, dtcb, kernel_base)  \
	.set	noat ; \
	.set 	noreorder ; \
	sd	sp, T_STACK_POINTER-TCBO(stcb) ; \
	ld	sp, T_ASID(dtcb) ; \
	ld	AT, T_GPT_POINTER(dtcb) ; \
	bgez	sp, 255f ; \
	dmtc0	sp, C0_ENTRYHI ; \
	sd	AT, K_GPT_POINTER(kernel_base) ; \
	ld	sp, T_STACK_POINTER(dtcb) ; \
	daddiu	AT, dtcb, TCB_SIZE ; \
	jal	asid_get ; \
	sd	AT, K_STACK_BOTTOM(kernel_base) ; \
	b	254f	 ; \
	nop ; \
255:	sd	AT, K_GPT_POINTER(kernel_base) ; \
	ld	sp, T_STACK_POINTER(dtcb) ; \
	daddiu	AT, dtcb, TCB_SIZE ; \
	sd	AT, K_STACK_BOTTOM(kernel_base) ; \
254: ; \
	.set	reorder ; \
	.set	at



/****************************************************************************
 *
 * ins_list(tcb, tcb link, kern base, list head)
 *
 * Note: list end must be -1, tcb link = 0 means not in list yet
 *
 ****************************************************************************/

#define  ins_list(tcb, tcb_link, kern_base, list_head) \
	.set	noat ; \
	ld	AT, tcb_link(tcb) ; \
	bne	AT, zero, 255f ; \
	ld	AT, list_head(kern_base) ; \
	sd	AT, tcb_link(tcb) ; \
	sd	tcb, list_head(kern_base) ; \
255:	 ; \
	.set	at

/****************************************************************************
 *
 * syscall_ret()
 *
 ****************************************************************************/

#if 0
# define syscall_ret_tmp \
	lw	k0, (sp)   /* status */ ; \
	li	k1, ~(ST_KSU| ST_IE) /* set EXL first */ ; \
	and	k1, k0, k1 ; \
	mtc0	k1, C0_STATUS
#else
# define syscall_ret_tmp
#endif

#define  syscall_ret \
	.set noreorder ; \
	.set noat ; \
	syscall_ret_tmp ; \
	mfc0	k0, C0_STATUS ; \
	ori	k0, k0, ST_EXL ; \
	mtc0	k0, C0_STATUS ; \
	li	k1, 0x0fffff00 ; \
	and	k0, k0, k1 ; \
	lbu	k1, (sp) ; \
	or	k0, k0, k1 ; \
 ; \
	ld	k1, 8(sp)  /* EPC */ ; \
	dmtc0   k1, C0_EPC ; \
	mtc0	k0, C0_STATUS /* now EXL is set, change KSU */ ; \
	ld	sp, 16(sp) ; \
	eret ; \
	.set at ; \
	.set reorder

/****************************************************************************
 *
 * ins_wakeup(timeout, tcb, kbase)
 *
 * insert tcb in appropriate wakeup list
 *
 ****************************************************************************/

#define  ins_wakeup(timeout, tcb, kbase) \
	.set noat ; \
	ld	AT, K_CLOCK(kbase) ; \
	daddu	AT, timeout, AT ; \
	sd	AT, T_WAKEUP(tcb) ; \
	 ; \
	daddiu	AT, timeout, -SOON_TIME ; \
	blez	AT, 254f ; \
 ; \
	daddiu	AT, timeout, -LATE_TIME ; \
	blez	AT, 253f ; \
	 ; \
	/* enter in late wakeup list */ ; \
	ins_list(tcb, T_LATE_WAKEUP_LINK, kbase, K_LATE_WAKEUP_LIST) ; \
	b	252f ; \
 ; \
254:	/* enter in soon wakeup list */ ; \
	ins_list(tcb, T_SOON_WAKEUP_LINK, kbase, K_SOON_WAKEUP_LIST)	 ; \
	b	252f ; \
	 ; \
253:   /* enter in wakeup list */ ; \
	ins_list(tcb, T_WAKEUP_LINK, kbase, K_WAKEUP_LIST)	 ; \
	 ; \
252:	 ; \
	.set	at

/****************************************************************************
 *
 * ins_int_list(tcb base, kernel base)
 *
 * insert tcb into interrupt list
 *
 ****************************************************************************/
	
#define  ins_int_list(tcb_base, kernel_base) \
	.set noat ; \
	ld	AT, K_INT_LIST(kernel_base) ; \
	sd	AT, T_INT_LINK(tcb_base) ; \
	sd	tcb_base, K_INT_LIST(kernel_base)	 ; \
	.set at

/****************************************************************************
 *
 * ins_sendq_end(sending tcb, receiving tcb)
 *
 * NOTE: 	next = head->tail
 *		prev = tail->head
 *
 ****************************************************************************/

#define  ins_sendq_end(sending_tcb, receiving_tcb) \
	.set noat ; \
	ld	AT, T_SNDQ_END(receiving_tcb) ; \
	bne	AT, zero, 255f ; \
		sd	sending_tcb, T_SNDQ_START(receiving_tcb) ; \
		sd	sending_tcb, T_SNDQ_END(receiving_tcb) ; \
		sd	zero, T_SNDQ_NEXT(sending_tcb) ; \
		sd	zero, T_SNDQ_PREV(sending_tcb) ; \
		b	254f ; \
255:	sd	zero, T_SNDQ_NEXT(sending_tcb) ; \
	sd	AT, T_SNDQ_PREV(sending_tcb) ; \
	sd	sending_tcb, T_SNDQ_NEXT(AT) ; \
	sd	sending_tcb, T_SNDQ_END(receiving_tcb) ; \
254: ; \
	.set at

/****************************************************************************
 *
 * rem_sendq(sending tcb, receiving tcb, temp reg)
 *
 ****************************************************************************/

#define rem_sendq(sending_tcb, receiving_tcb, temp_reg) \
	.set	noat ; \
	ld	AT, T_SNDQ_NEXT(sending_tcb) ; \
	bne	AT, zero, 255f ; \
		ld	AT, T_SNDQ_PREV(sending_tcb) ; \
		sd	AT,  T_SNDQ_END(receiving_tcb) ; \
		b	254f ; \
255:	ld	temp_reg, T_SNDQ_PREV(sending_tcb) ; \
	sd	temp_reg, T_SNDQ_PREV(AT) ; \
 ; \
254:	ld	AT, T_SNDQ_PREV(sending_tcb) ; \
	bne	AT, zero, 255f ; \
		ld	AT, T_SNDQ_NEXT(sending_tcb) ; \
		sd	AT, T_SNDQ_START(receiving_tcb) ; \
		b	254f ; \
255:	ld	temp_reg, T_SNDQ_NEXT(sending_tcb) ; \
	sd	temp_reg, T_SNDQ_NEXT(AT) ; \
 ; \
254:	.set at

/****************************************************************************
 *
 * ins_busy_list(tcb, kern base, temp reg)
 *
 * insert in busy list only if not already inside
 *
 ****************************************************************************/

#define ins_busy_list(tcb, kern_base, temp_reg) \
	.set	noat ; \
	 ; \
	/* check if already in busy list */ ; \
	ld	AT, T_BUSY_LINK(tcb) ; \
	bne	AT, zero, 254f /* already in a queue */ ; \
 ; \
	/* check if timeslice is zero */ ; \
	lhu	AT, T_TIMESLICE(tcb) ; \
	beq	AT, zero, 255f ; \
 ; \
	/* insert into appropriate queue */ ; \
	lbu	temp_reg, T_TSP(tcb) ; \
	sll	temp_reg, 3 ; \
	daddu	temp_reg, temp_reg, kern_base ; \
 ; \
	ld	AT, K_PRIO_BUSY_LIST(temp_reg) ; \
	sd	tcb, K_PRIO_BUSY_LIST(temp_reg) ; \
	bne	AT, zero, 253f ; \
	 ; \
	/* insert tcb into empty queue */ ; \
	sd	tcb, T_BUSY_LINK(tcb) ; \
	b	254f ; \
	nop ; \
 ; \
253:	/* insert tcb at tail of existing queue */ ; \
	ld	temp_reg, T_BUSY_LINK(AT) ; \
	sd	temp_reg, T_BUSY_LINK(tcb) ; \
	sd	tcb, T_BUSY_LINK(AT) ; \
254: ; \
	/* reset remaining time slice & current priority values */ ; \
	lhu	temp_reg, T_TIMESLICE(tcb) ; \
	sh	temp_reg, T_REM_TIMESLICE(tcb) ; \
	lbu	temp_reg, T_TSP(tcb) ; \
	sb	temp_reg, T_CTSP(tcb) ; \
255: ; \
	.set at ; \
	

/****************************************************************************
 *
 * init_tcb(tcb base)
 *
 * initialize variables in new tcb block
 *
 ****************************************************************************/

#define init_tcb(tcb_base) \
	sd	zero, T_SNDQ_END(tcb_base) ; \
	sd	zero, T_WAKEUP_LINK(tcb_base) ; \
	sd	zero, T_BUSY_LINK(tcb_base) ; \
	sd	zero, T_INT_LINK(tcb_base) ; \
	sd	zero, T_WFOR(tcb_base) ; \
	sd	zero, T_SNDQ_START(tcb_base) ; \
	sd	zero, T_CPU_TIME(tcb_base) ; \
	/* T_STACK_POINTER */ ; \
	sd	zero, T_ASID(tcb_base) ; \
	/* gpt pointer */ ; \
	/* T_MYSELF */ ; \
	/* T_FINE_STATE */ ; \
	/* T_COARSE_STATE */ ; \
	sd	zero, T_RECV_DESC(tcb_base) ; \
	/* T_PRESENT_NEXT */ ; \
	sd	zero, T_CHILD_TASK(tcb_base) ; \
	sd	zero, T_REM_TIMESLICE(tcb_base) ; \
	/* T_WAKEUP (dont care) */ ; \
	sd	zero, T_SOON_WAKEUP_LINK(tcb_base) ; \
	sd	zero, T_LATE_WAKEUP_LINK(tcb_base) ; \
	sd	zero, T_SNDQ_NEXT(tcb_base) ; \
	sd	zero, T_SNDQ_PREV(tcb_base) ; \
	/* pager */ ; \
	/* preempter */ ; \
	/* comm_parter */ ; \
	/* wdw_addr */ ; \
	sd	zero, T_INTERRUPT_MASK(tcb_base) ; \
	sw	zero, T_STACKED_FINE_STATE(tcb_base) ; \
	/* T_STACKED_COMM_PRTNR */ ; \
	sd	zero, T_SISTER_TASK(tcb_base) ; \
	
	


/****************************************************************************
 *
 * ins_pres_list(new tcb base, existing base)
 *
 * insert tcb in present double linked list
 *
 ****************************************************************************/

#define ins_pres_list(new_base, existing_base) \
	.set	noat ; \
	ld	AT, T_PRESENT_NEXT(existing_base) ; \
	sd	AT, T_PRESENT_NEXT(new_base) ; \
	sd	new_base, T_PRESENT_NEXT(existing_base) ; \
	.set	at ; \
	
	
/****************************************************************************
 *
 * to_next_thread(kernel_base)
 *
 * switch from current thread to next runnable thread on busy list 
 *
 ****************************************************************************/

#define to_next_thread(kernel_base) \
	move	s0, kernel_base ; \
	move	a0, kernel_base ; \
	jal	get_next_thread ; \
	tcbtop(t0) ; \
	thread_switch_fast(t0, v0, s0) ; \
	ld	ra, (sp) ; \
	jr	ra ; \
	

/****************************************************************************
 *
 * make_busy(tcb, return_code)
 *
 * mark the given thread busy with the given return code
 *
 ****************************************************************************/

#define make_busy(tcb, return_code) \
	.set	noat ; \
	daddiu	AT, tcb, TCB_SIZE + ST_EX_V0 ; \
	sd	return_code, (AT) ; \
	daddiu	AT, tcb, TCB_SIZE - ST_EX_SIZE -8 ; \
	sd	AT, T_STACK_POINTER(tcb) ; \
	dla	return_code, preempt_ret ; \
	sd	return_code, (AT) ; \
	li	AT, FS_BUSY ; \
	sw	AT, T_FINE_STATE(tcb) ; \
	.set	at ; \
	

/****************************************************************************
 *
 * trace(string) macro to change led display for tracing
 *
 ****************************************************************************/

#if defined(KDEBUG) && defined(P4000)
# define trace(string) \
	.data ; \
1: 	; \
	.ascii "string" ; \
	.text ; \
	lui	k0, KERNEL_BASE ; \
	sd	t0, K_TRACE_REG_SAVE(k0) ; \
	dla	k0, 1b ; \
	dli	k1, PHYS_TO_CKSEG1(ALPHN_CHAR_BASE) ; \
	lbu	t0, (k0) ; \
	sll	t0, 24 ; \
	sw	t0, 12(k1) ; \
	lbu	t0, 1(k0) ; \
	sll	t0, 24 ; \
	sw	t0, 8(k1) ; \
	lbu	t0, 2(k0) ; \
	sll	t0, 24 ; \
	sw	t0, 4(k1) ; \
	lbu	t0, 3(k0) ; \
	sll	t0, 24 ; \
	sw	t0, (k1) ; \
	lui	k0, KERNEL_BASE ; \
	ld	t0, K_TRACE_REG_SAVE(k0)
#elif defined(KDEBUG) && defined(U4600)
# define trace(string) \
	.data ; \
1: 	; \
	.ascii "string" ; \
	.text ; \
	lui	k0, KERNEL_BASE ; \
	sd	t0, K_TRACE_REG_SAVE(k0) ; \
	dla	k0, 1b ; \
	dli	k1, PHYS_TO_CKSEG1(LED_BASE) ; \
	lbu	t0, (k0) ; \
	sb	t0, 3(k1) ; \
	lbu	t0, 1(k0) ; \
	sb	t0, 2(k1) ; \
	lbu	t0, 2(k0) ; \
	sb	t0, 1(k1) ; \
	lbu	t0, 3(k0) ; \
	sb	t0, (k1) ; \
	lui	k0, KERNEL_BASE ; \
	ld	t0, K_TRACE_REG_SAVE(k0)
#else
# define trace(string)
#endif


/****************************************************************************
 *
 * break() call the debugger preserving everything except k0, k1
 *
 ****************************************************************************/

#define break \
	lui	k0, KERNEL_BASE ; \
	sd	ra, K_TRACE_REG_SAVE(k0) ; \
	jal	dbg ; \
	lui	k0, KERNEL_BASE ; \
	ld	ra,  K_TRACE_REG_SAVE(k0) ; \
	
/****************************************************************************
 *
 * cbreak() conditional call the debugger preserving everything except k0, k1
 *
 ****************************************************************************/

#ifdef KDEBUG
# define cbreak \
	lui	k0, KERNEL_BASE ; \
	ld	k1, K_BREAK_ON(k0) ; \
	beq	k1, zero, 255f ; \
	sd	ra, K_TRACE_REG_SAVE(k0) ; \
	jal	dbg ; \
	lui	k0, KERNEL_BASE ; \
	lui	k0, KERNEL_BASE ; \
	ld	ra,  K_TRACE_REG_SAVE(k0) ; \
255:
#else
# define cbreak
#endif
	

/****************************************************************************
 *
 * receive_timeout(timeout, rcv_exp, rtime)
 *
 * decode the receive timeout value
 *
 ****************************************************************************/

#define receive_timeout(timeout, rcv_exp, rtime) \
	.set	noat ; \
	dli	AT, L4_RCV_MAN_MASK ; \
	and	AT, timeout, AT ; \
	dsrl	AT, AT, 4  /* return timeout in 1024 us units */ ; \
	dsrlv	AT, AT, rcv_exp ; \
	dsrlv	rtime, AT, rcv_exp ; \
	.set	at ; \
	

/****************************************************************************
 *
 * send_timeout(timeout, snd_exp, stime)
 *
 * decode the send timeout value
 *
 ****************************************************************************/

#define send_timeout(timeout, snd_exp, stime) \
	.set	noat ; \
	dli	AT, L4_SND_MAN_MASK ; \
	and	AT, timeout, AT ; \
	dsll	AT, AT, 4  /* return timeout in 1024 us units */ ; \
	dsrlv	AT, AT, snd_exp ; \
	dsrlv	stime, AT, snd_exp ; \
	.set	at ; \
	

/****************************************************************************
 * int_preempt(src, inth, kern, tmp)
 *
 * stack an interrupted thread and reload the kernel scheduling vars
 *   	(only stack if current remaining timeslice is greater than zero)
 * src: tcb base of thread being interrupted
 * inth: tcb base of interrupt handler thread
 * kern: kernel base 
 * tmp: temprary reg
 ****************************************************************************/

#define int_preempt(src, inth, kern, tmp) \
	.set	noat ; \
	lw 	AT, K_TIMESLICE(kern) ; \
	beq	zero, AT, 254f ; \
	nop ; \
	sh	AT, T_REM_TIMESLICE(src) ; \
	lw	AT, K_PRIORITY(kern) ; \
	sb	AT, T_CTSP(src) ; \
	ins_int_list(src, kern) ; \
	.set noat ; \
	b	255f ; \
	nop ; \
254: ; \
	ins_busy_list(src, kern, tmp) ; \
	.set noat ; \
255:	 ; \
	/* reload kernel vars */ ; \
	lhu	AT, T_TIMESLICE(inth) ; \
	sw	AT, K_TIMESLICE(kern) ; \
	lbu	AT, T_CTSP(inth) ; \
	sw	AT, K_PRIORITY(kern)   ; \
	.set at








