/****************************************************************************
 *      $Id: exc.S,v 1.7 2002/08/01 03:05:50 cls Exp $
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
#include <kernel/panic.h>
#include <kernel/machine.h>
#include <kernel/kernel.h>
#include <l4/types.h>
#include <l4/syscalls.h>
#include <l4/ipc.h>
#include "macros.h"
	
	.rdata
msg_syscall:
	.asciiz "syscall other"
msg_ipc:
	.asciiz "syscall ipc"
one:	.ascii	"one "
two:	.ascii	"two "
three:	.ascii	"thre"
four:	.ascii	"four"

	.data
here_msg:	
	.asciiz	"here !!"	

PROC(exc_init)
		/* load some code to redirect the excpt */
	dla	a0, gen_exc_start
	dla     a3, gen_exc_end
	dli	a1, GEN_EXCPT_VEC
1:
	lw	a2, (a0)
	addiu	a0, a0, 4
	sw	a2, (a1)
	addiu   a1, a1, 4
	bne     a0, a3, 1b
	
	/* load xtlb refill handler */
	dla	a0, xtlb_refill_start
	dla     a3, xtlb_refill_end
	dli	a1, XTLB_VEC  
1:
	lw	a2, (a0)
	addiu	a0, a0, 4
	sw	a2, (a1)
	addiu   a1, a1, 4
	bne     a0, a3, 1b

	/* build a jump table for exceptions */
	lui	a1, KERNEL_BASE
	daddiu	a1, a1, K_EXC_JMP_TABLE
	dla	a0, exc_int
	sd	a0, (a1)
	dla	a0, exc_mod
	sd	a0, 8(a1)
	dla	a0, exc_tlbl
	sd	a0, 16(a1)
	dla	a0, exc_tlbs
	sd	a0, 24(a1)
	dla	a0, exc_adel
	sd	a0, 32(a1)
	dla	a0, exc_ades
	sd	a0, 40(a1)
	dla	a0, exc_ibe
	sd	a0, 48(a1)
	dla	a0, exc_dbe
	sd	a0, 56(a1)
	dla	a0, exc_not_defined /* syscalls handled differently */
	sd	a0, 64(a1)
	dla	a0, exc_bp
	sd	a0, 72(a1)
	dla	a0, exc_ri
	sd	a0, 80(a1)
	dla	a0, exc_cpu
	sd	a0, 88(a1)
	dla	a0, exc_ov
	sd	a0, 96(a1)
	dla	a0, exc_tr
	sd	a0, 104(a1)
	dla	a0, exc_not_defined
	sd	a0, 112(a1)
	dla	a0, exc_fpe
	sd	a0, 120(a1)
	jr	ra
END(exc_init)
	
PROC(excpt_redirect)
	.set noreorder
excpt_r_start:	
	dla	k0, gen_exc
	jr	k0
	nop
excpt_r_end:	
END(excpt_redirect)

	.data
msg:	.asciiz	"sycl"
	
PROC(gen_exc)
gen_exc_start:
	trace(gnex)
	.set noreorder
	.set noat
	mfc0	k0, C0_CAUSE
	mfc0	k1, C0_STATUS
	andi	k0, CA_EXC_CODE /* check if syscall first */
	subu	k0, CA_Sys
	beq	k0, zero, 1f
	lui	k0, KERNEL_BASE
	j	other_excpt
	nop
1:	move    t0, k1
	srl     k1, 5  /* clear IE, EXL, ERL, KSU */
	sll	k1, 5
	mtc0	k1, C0_STATUS
	andi    k1, t0, ST_KSU  /* load kernel sp if exc from user mode */
	beq	k1, zero, 1f
	move	t2, sp
	ld	sp,  K_STACK_BOTTOM(k0)
1:	dmfc0	t1, C0_EPC      /* stack sp, EPC, status */
	sd	t2, -8(sp)
	daddiu	t1, t1, 4
	sd	t1, -16(sp)
	sb	t0, -24(sp) /* sp must be 8 byte aligned */
	bne	AT, zero, 1f
	dsubu	sp, 24
	j	k_ipc
	tcbtop(t8)
	/* use a jump table here for other syscalls */
1:	trace(esys)

	slti	t1, AT, MAX_SYSCALL_NUMBER + 1
	beq	t1, zero, 2f
	dsll	AT, 3
	daddu	t0, k0, AT
	ld	t0, K_SYSCALL_JMP_TABLE(t0)
	jr	t0
	ori	t8, sp, TCBO /* load source TCB base + TCBO*/ 
2:	.set at
	.set reorder
	syscall_ret 
gen_exc_end:
END(gen_exc)

PROC(other_excpt)
fail_tlb_rfl_ent:
	.globl fail_tlb_rfl_ent
	.set noreorder
	.set noat
	mfc0	k1, C0_STATUS
	/* save some registers to work with */
	sd	s0, K_S0_SAVE(k0)
	sd	s1, K_S1_SAVE(k0)
	sd	s2, K_S2_SAVE(k0)
	sd	s3, K_S3_SAVE(k0)
	sd	s4, K_S4_SAVE(k0)
	move    s0, k1                                     /* s0 STATUS   */
	srl     k1, 5  /* clear IE, EXL, ERL, KSU */
	sll	k1, 5
	mtc0	k1, C0_STATUS
	andi    k1, s0, ST_KSU  /* load kernel sp if exc from user mode */
	beq	k1, zero, 1f
	move	s1, sp                                     /* s1 STACK    */
		ld	sp,  K_STACK_BOTTOM(k0)
1:	dmfc0	s2, C0_EPC      /* stack sp, EPC, status *//* s2 EPC      */
	dmfc0	s3, C0_BADVADDR                            /* s3 BADVADDR */
	mfc0	s4, C0_CAUSE                               /* s4 CAUSE    */

	sd	s1, -8(sp)
	sd	s2, -16(sp)
	sb	s0, -24(sp) /* sp must be 8 byte aligned */
	
	/* dump the registers on the stack */
	sd	AT, -32(sp)
	sd	v0, -40(sp)
	sd	v1, -48(sp)
	sd	a0, -56(sp)
	sd	a1, -64(sp)
	sd	a2, -72(sp)
	sd	a3, -80(sp)
	sd	a4, -88(sp)
	sd	a5, -96(sp)
	sd	a6, -104(sp)
	sd	a7, -112(sp)
	sd	t0, -120(sp)
	sd	t1, -128(sp)
	sd	t2, -136(sp)
	sd	t3, -144(sp)

	lui	k0, KERNEL_BASE
	ld	t0, K_S0_SAVE(k0)
	ld	t1, K_S1_SAVE(k0)
	sd	t0, -152(sp) /* s0 */
	ld	t0, K_S2_SAVE(k0)
	sd	t1, -160(sp) /* s1 */
	ld	t1, K_S3_SAVE(k0)
	sd	t0, -168(sp) /* s2 */
	ld	t0, K_S4_SAVE(k0)
	sd	t1, -176(sp) /* s3 */
	sd	t0, -184(sp) /* s4 */
	
	sd	s5, -192(sp)
	sd	s6, -200(sp)
	sd	s7, -208(sp)
	sd	t8, -216(sp)
	sd	t9, -224(sp)
	/* k0, k1 not stacked */
	sd	gp, -232(sp)
	/* sp already stacked */
	sd	s8, -240(sp)
	sd	ra, -248(sp)

	/* stack hi/lo */
	mflo	t0
	sd	t0, -256(sp)
	mfhi	t0
	sd	t0, -264(sp)
	
	daddiu	sp, sp, -(ST_EX_SIZE)

	/* at this point we have saved enough state to do what we
	like from now on with registers, even call C code  */
	/* use jump table to call appropriate exception handler */
	trace(eser)
	andi	t0, s4, CA_EXC_CODE
	dsll	t0, 1
	daddu	t0, t0, k0
	ld	k1, K_EXC_JMP_TABLE(t0)
	nop
	jr	k1
	nop
END(other_excpt)

PROC(other_excpt_ret)
	/* at this point we have handled the exception, maybe even context
	switched. So we now load the registers from whatever stack we have
	and return */

	ld	t0, (sp) /* HI */
	mthi	t0
	ld	t0, 8(sp) /* LO */
	mtlo	t0
	ld	ra, 16(sp)
	ld	s8, 24(sp)
	ld	gp, 32(sp)
	ld	t9, 40(sp)
	ld	t8, 48(sp)
	ld	s7, 56(sp)
	ld	s6, 64(sp)
	ld	s5, 72(sp)
	ld	s4, 80(sp)
	ld	s3, 88(sp)
	ld	s2, 96(sp)
	ld	s1, 104(sp)
	ld	s0, 112(sp)
	ld	t3, 120(sp)
	ld	t2, 128(sp)
	ld	t1, 136(sp)
	ld	t0, 144(sp)
	ld	a7, 152(sp)
	ld	a6, 160(sp)
	ld	a5, 168(sp)
	ld	a4, 176(sp)
	ld	a3, 184(sp)
	ld	a2, 192(sp)
	ld	a1, 200(sp)
	ld	a0, 208(sp)
	ld	v1, 216(sp)
	ld	v0, 224(sp)
	ld	AT, 232(sp)

#if 0	
	lw	k0, 240(sp) /* status */
	dli	k1, ~(ST_KSU|ST_IE) /* set EXL first */
	and	k1, k0, k1
	mtc0	k1, C0_STATUS
#endif
	mfc0	k0, C0_STATUS
	ori	k0, k0, ST_EXL
	mtc0	k0, C0_STATUS
	li	k1, 0x0fffff00
	and	k0, k0, k1
	lbu	k1, 240(sp)
	or	k0, k0, k1
	
	ld	k1, 248(sp) /* EPC */
	dmtc0   k1, C0_EPC
	mtc0	k0, C0_STATUS
	ld	sp, 256(sp)
	eret

	.set at
	.set reorder
END(other_excpt_ret)

	.rdata
msg_exc_not_defined:
	.asciiz	"Excpt: Not Defined?????"

PROC(exc_int)
	.set	noreorder
	/* C0_CAUSE in s4 */
	mfc0	t0, C0_STATUS
	and	s0, t0, s4
	dsll	s0, s0, 48
	bgez	s0, 1f
	dsll	s0, 1
	jal	int_ip7
	nop
1:	bgez	s0, 2f
	dsll	s0, 1
	jal	int_ip6
	nop
2:	bgez	s0, 3f
	dsll	s0, 1
	jal	int_ip5
	nop
3:	bgez	s0, 4f
	dsll	s0, 1
	jal	int_ip4
	nop
4:	bgez	s0, 5f
	dsll	s0, 1
	jal	int_ip3
	nop
5:	bgez	s0, 6f
	dsll	s0, 1
	jal	int_ip2
	nop
6:	bgez	s0, 7f
	dsll	s0, 1
	jal	int_ip1
	nop
7:	bgez	s0, 8f
	nop
	jal	int_ip0
	nop
8:		
	j	other_excpt_ret
	nop
	.set	reorder
END(exc_int)


/* offsets of registers on the kernel stack (keep synched with other_excpt!) */
	.align	2
other_excpt_offsets:
	.short	   0,  -32,  -40,  -48,  -56,  -64,  -72,  -80
	.short	 -88,  -96, -104, -112, -120, -128, -136, -144
	.short	-152, -160, -168, -176, -184, -192, -200, -208
	.short	-216, -224,    0,    0, -232,   -8, -240, -248
	
/*
 * Work around IDT 79R4600 rev. 2.0 erratum 7.  The CPU sometimes generates
 * incorrect BVA and EntryHi addresses in a Mod exception taken on the second
 * last instruction in a page.  Synthesize the correct BVA and jump back to
 * exc_tlbs().  CHECK: does this work with SWL and SWR? 
 */
PROC(idt_fixup)
	/* load word containing store instruction */
	lwu	t0, (s2)
	
	/* extract the 5-bit basereg field, scaled by sizeof(short) */
	srl	t1, t0, 20
	andi	t1, 0x003e
	
	/* extract the 16-bit displacement */
	andi	s3, t0, 0xffff
	
	/* if basereg is $0 (!), effective address is just displacement */
	beqz	t1, exc_tlbs
	
	/* retrieve stacked register (garbage for $0, $26, or $27) */
	lh	t0, other_excpt_offsets(t1)
	daddu	t0, sp
	ld	t0, ST_EX_SIZE(t0)
	
	/* compute effective address by adding displacement to basereg */
	daddu	s3, t0
	b	exc_tlbs
END(idt_fixup)


PROC(exc_mod)
	/* fix-up if exception was taken on second last instruction in a page */
	andi	t0, s2, 0x0fff
	dli	t1, 0x0ffc
	beq	t0, t1, idt_fixup	/* jumps back to exc_tlbs afterwards */

	.globl exc_tlbs
exc_tlbs:

	
	trace(tlbs)
	.set	noreorder

	bltz	s3, 3f  /* tcb faults handled differently */
	move	t2, sp

	tcbtop(a0) /* check if page fault in long ipc */
	lw	t0, T_FINE_STATE-TCBO(a0)
	andi	a1, t0, FS_LOCKS
	beq	a1, zero, 4f

	/* page fault is in long ipc */
	dsrl	a1, s3, 62

	bne	a1, zero, window_fault
	nop
	dla	t3, ipc_fault_ret
	
	/* stack state for kill and exreg to unwind */
	sw	t0, T_STACKED_FINE_STATE-TCBO(a0)
	ld	t0, T_COMM_PARTNER-TCBO(a0)
	sd	t0, T_STACKED_COMM_PRTNR-TCBO(a0)

	/* use receivers recv pf timeout */
	li	a2, 0x01010000
	lw	t1, T_TIMEOUT(t0)
	lw	a3, T_TIMEOUT-TCBO(a0) /* preserve my pf timeouts */
	andi	a3, a3, 0xff00
	andi	t1, t1, 0x0f00
	srl	t1, t1, 4
	or	a2, a2, t1
	srl	t1, t1, 4
	or	a2, a2, t1
	or	a2, a2, a3

	daddiu	sp, sp, -8
	move	t2, sp
	sd	s3, (sp)
	b	5f
	nop
	
	/* nasty problem: we need to enable interrupts here as tlb
exceptions are higher priority than interrupts and the situation arises
that we can page fault on code with an invalid pager and we endless loop
with interrupts disabled */
	
4:	dla	t3, other_excpt_ret
	dli	a2, L4_IPC_NEVER	

5:	mfc0	t0, C0_STATUS
	li	t1, ST_IE
	or	t1, t0, t1
	mtc0	t1, C0_STATUS 

	daddiu	sp, sp, -24    /* fake a stack such that syscall_ret actually 
				does an exception return */
	sb	t0, (sp)
	sd	t3, 8(sp)
	sd	t2, 16(sp)

	dli	s0, ~(L4_FPAGE_RW_MASK | L4_FPAGE_GRANT_MASK)
	and	s0, s0, s3
	ori	s0, s0, L4_FPAGE_RW_MASK
	
	move	s1, s2


	tcbtop(t8)
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_SHORT_FPAGE | (L4_WHOLE_ADDRESS_SPACE << 2 )
	ld	a4, T_PAGER_TID-TCBO(t8)
	move	a5, a4
	mtc0	t0, C0_STATUS

	dli	a6, 0

	j	k_ipc
	move	a5, a4


	.set	reorder

3:
	/* we have a fault in tcb space so we map a shared invalid page
         */
	lui	a0, KERNEL_BASE
	ld	a0, K_GPT_POINTER(a0)
	move	a1, s3
	dli	a2, INVALID_TCB_BASE

	jal	vm_tcb_insert
	
	j	other_excpt_ret
END(exc_mod)


	.data
test_msg:
	.asciiz "testing??"
seg_msg:
	.asciiz "segmentation fault"
PROC(exc_tlbl)
	
	/* check tlb not stale by checking if page table actually valid
	*/
	trace(tlbl)
	.set	noreorder
	bltz	s3, 3f  /* tcb faults handled differently */
	move	t2, sp
	
	tcbtop(a0) /* check if page fault in long ipc */
	lw	t0, T_FINE_STATE-TCBO(a0)
	andi	a1, t0, FS_LOCKS
	beq	a1, zero, 4f

	/* page fault is in long ipc */
	dsrl	a1, s3, 62
	bne	a1, zero, window_fault
	nop
	dla	t3, ipc_fault_ret
	/* stack state for kill and exreg to unwind */
	sw	t0, T_STACKED_FINE_STATE-TCBO(a0)
	ld	t0, T_COMM_PARTNER-TCBO(a0)
	sd	t0, T_STACKED_COMM_PRTNR-TCBO(a0)
	
	li	a2, 0x01010000
	lw	t1, T_TIMEOUT(t0)
	lw	a3, T_TIMEOUT-TCBO(a0)  /* preserve my pf timeouts */
	andi	a3, a3, 0xff00
	andi	t1, t1, 0x0f00
	srl	t1, t1, 4
	or	a2, a2, t1
	srl	t1, t1, 4
	or	a2, a2, t1
	or	a2, a2, a3
	
	daddiu	sp, sp, -8
	move	t2, sp
	sd	s3, (sp)
	b	5f
	nop

	
	/* nasty problem: we need to enable interrupts here as tlb
exceptions are higher priority than interrupts and the situation arises
that we can page fault on code with an invalid pager and we endless loop
with interrupts disabled */

4:	dla	t3, other_excpt_ret
	dli	a2, L4_IPC_NEVER	
	
5:	mfc0	t0, C0_STATUS
	li	t1, ST_IE
	or	t1, t0, t1
	mtc0	t1, C0_STATUS

	daddiu	sp, sp, -24    /* fake a stack such that syscall_ret actually 
				does an exception return */
	sb	t0, (sp)
	sd	t3, 8(sp)
	sd	t2, 16(sp)
		
	dli	s0, ~(L4_FPAGE_RW_MASK | L4_FPAGE_GRANT_MASK)
	and	s0, s0, s3
	move	s1, s2


	tcbtop(t8)
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_SHORT_FPAGE | (L4_WHOLE_ADDRESS_SPACE << 2 )
	ld	a4, T_PAGER_TID-TCBO(t8)
	mtc0	t0, C0_STATUS /* disable interrupts again */
	dli	a6, 0

	/* don't care about potential infinite fault loop ??*/
	/* it's the pagers problem */
	cbreak 

	j	k_ipc
	move	a5, a4


	.set	reorder

3:
	/* we have a fault in tcb space so we map a shared invalid page
         */
	lui	a0, KERNEL_BASE
	ld	a0, K_GPT_POINTER(a0)
	move	a1, s3
	dli	a2, INVALID_TCB_BASE
	cbreak 
	jal	vm_tcb_insert
	
	j	other_excpt_ret
END(exc_tlbl)

PROC(ipc_fault_ret)
	/* called with v0 = receive fpage result */
	/* sender fault address on stack */
	tcbtop(s2)

	/* unstack ipc state */
	lw	t0, T_STACKED_FINE_STATE-TCBO(s2)
	sw	t0, T_FINE_STATE-TCBO(s2)
	sd	zero, T_STACKED_FINE_STATE-TCBO(s2)
	ld	s3, T_STACKED_COMM_PRTNR-TCBO(s2)
	sd	s3, T_COMM_PARTNER-TCBO(s2)
	
	ld	s0, (sp) /* original fault address */
	daddiu	sp, sp, 8

	/* check if ipc had error */
	andi	t0, v0, L4_IPC_ERROR_MASK
	bne	t0, zero, 1f

	/* check if page was actually mapped */
	ld	a0,  T_GPT_POINTER-TCBO(s2)
	move	a1, s0
	jal	vm_lookup_pte
	
	beq	v0, zero, 1f
	lw	a0, (v0)
	and	t1, a0, EL_Valid
	beq	t1, zero, 1f   /* page must be valid  */

	
	j	other_excpt_ret

1:	/* page fault either failed or timed out*/
	/* break off ipc with and SEND PAGE FAULT TIMEOUT */
	
	/* unwind sender stack to simple syscall */
	daddiu	sp, s2, 1 - 24

	dli	v0, L4_IPC_RESNDPFTO
	move	t8, s2 /* set stcb */
	move	t9, s3 /* set dtcb */
	ld	v1, T_MYSELF-TCBO(t8)

	b	send_only_short
END(ipc_fault_ret)

PROC(window_fault)
	trace(wflt)

	/* first figure out real faulting address in receiver */
	/* a0 has tcbtop */
	ld	t0, T_MYSELF-TCBO(a0)
	dli	t1, TID_THREAD_MASK
	and	t0, t0, t1
	dsrl	t0, t0, TID_THREAD_SHIFT
	dsll	t0, t0, RECV_WINDOW_SHIFT
	dli	t1, RECV_WINDOW_BASE
	daddu	t0, t1, t0 /* t0 now contains base of window */

	dsubu   a1, s3, t0 /* a1 now has offset from window base */
	ld	a2,  T_WDW_MAP_ADDR-TCBO(a0) 
	daddu	s5, a1, a2 /* a1 now has address in receiver */

	ld	a2, T_COMM_PARTNER-TCBO(a0)
	ld	a0, T_GPT_POINTER(a2)
	move	a1, s5
	jal	vm_lookup_pte

	beq	v0, zero, 1f

	lw	a0, (v0) /* a0 has pte for page */
	li	t0, EL_Valid|EL_Dirty
	and	t1, a0, t0
	bne	t1, t0, 1f   /* page must be valid and writable */

	/* replace or inset entry into tlb */
	tcbtop(t8)
	ld	a1, T_ASID-TCBO(t8)
	dli	t1, ~(8192-1)
	and	a2, s3, t1
	or	a3, a2, a1  /* a1 now has entryhi */

	.set	noreorder
	dmtc0	a3, C0_ENTRYHI
	nop
	tlbp
	andi	t0, s3, 4096
	mfc0	t2, C0_INDEX
	bltz	t2, 2f /* test in or out of tlb */
	nop
	tlbr
	beq	t0, zero, 3f /* test odd or even */
	nop
	dmtc0	a0, C0_ENTRYLO1
	b	4f
	nop	
3:	dmtc0	a0,  C0_ENTRYLO0
	nop
4:	tlbwi
	j	other_excpt_ret
	nop




2:	/* not in tlb, so do write random */

	beq	t0, zero, 3f /* test if odd or even */
	nop
	dmtc0	zero,  C0_ENTRYLO0
	dmtc0	a0, C0_ENTRYLO1	
	nop
	tlbwr
	j	other_excpt_ret
	nop

3:	dmtc0	a0,  C0_ENTRYLO0
	dmtc0	zero,  C0_ENTRYLO1
	nop
	tlbwr
	j	other_excpt_ret
	nop	
	
	.set	reorder	
	
1:	/* page fault on receiver side */
	
	/* leave sender ready for restart */
	daddiu	sp, sp, -8
	dla	a0, window_fault_ret
	sd	a0, (sp)
	tcbtop(t8)
	li	t0, FS_LOCKS
	sw	t0, T_FINE_STATE-TCBO(t8)
	
	/* switch to receiver stack and address space */
	sd	sp, T_STACK_POINTER-TCBO(t8)
	ld	t8, T_COMM_PARTNER-TCBO(t8)
	ld	sp, T_STACK_POINTER(t8)

	/* switch the asid, check if new asid is valid */
	ld	t0, T_ASID(t8)
	.set	noreorder
	bgez	t0, 9f
	dmtc0	t0, C0_ENTRYHI
	jal	asid_get
	nop
	
9:	ld	t0, T_GPT_POINTER(t8)
	lui	t9, KERNEL_BASE
	sd	t0, K_GPT_POINTER(t9)
	
	/* stack state for kill and exreg to unwind */
	li	t0, FS_LOCKR
	sw	t0, T_STACKED_FINE_STATE(t8)
	ld	t0, T_COMM_PARTNER(t8)
	sd	t0, T_STACKED_COMM_PRTNR(t8)

	
	li	a2, 0x01010000
	lw	t1, T_TIMEOUT(t0)
	lw	a3, T_TIMEOUT(t8) /* preserve my pf timeouts */
	andi	a3, a3, 0xff00
	andi	t1, t1, 0xf000
	srl	t1, t1, 8
	or	a2, a2, t1
	srl	t1, t1, 4
	or	a2, a2, t1
	or	a2, a2, a3

	/* generate a page fault */
	daddiu	t2, sp, -16
	daddiu	sp, sp, -40    /* fake a stack such that syscall_ret actually 
					does an exception return */
	dla	t3, 1f
	mfc0	t0, C0_STATUS 
	sb	t0, (sp)
	sd	t3, 8(sp)
	sd	t2, 16(sp)
		
	dli	s0, ~(L4_FPAGE_RW_MASK | L4_FPAGE_GRANT_MASK)
	and	s0, s0, s5
	ori	s0, s0, L4_FPAGE_RW_MASK
	move	s1, zero
	sd	s5, 24(sp)
	sd	s3, 32(sp)

	tcbtop(t8)
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_SHORT_FPAGE | (L4_WHOLE_ADDRESS_SPACE << 2 )
	ld	a4, T_PAGER_TID-TCBO(t8)

	cbreak 

	j	k_ipc
	move	a5, a4


	.set	reorder

	
1:	
	/* unstack some needed values */

	ld	s0, (sp) /* receiver side address */
	ld	s1, 8(sp) /* window fault address */
	daddiu	sp, sp, 16
	/* unstack receiver state */
	tcbtop(t8)
	lw	t0, T_STACKED_FINE_STATE-TCBO(t8)
	sw	t0, T_FINE_STATE-TCBO(t8)
	sd	zero, T_STACKED_FINE_STATE-TCBO(t8)
	ld	t1, T_STACKED_COMM_PRTNR-TCBO(t8)
	sd	t1, T_COMM_PARTNER-TCBO(t8)
	li	t0, FS_LOCKS | FS_BUSY
	sw	t0, T_FINE_STATE(t1)
	lui	t2, KERNEL_BASE
	thread_switch_fast(t8, t1, t2)
END(window_fault)
PROC(window_fault_ret)
	/* called with v0 = receive fpage result,
	s0 receiver side fault address,
	s1 sender side fault address */
	daddiu	sp, sp, 8
	tcbtop(s2)
	ld	s3, T_COMM_PARTNER-TCBO(s2)
	andi	t0, v0, L4_IPC_ERROR_MASK
	bne	t0, zero, 1f
	ld	a0,  T_GPT_POINTER(s3)
	move	a1, s0

	jal	vm_lookup_pte
	beq	v0, zero, 1f
	lw	a0, (v0)
	li	t0, EL_Valid|EL_Dirty
	and	t1, a0, t0
	bne	t1, t0, 1f   /* page must be valid and writable */

	/* page fault worked so load sender tcb */

	/* replace or inset entry into tlb */
	ld	a1, T_ASID-TCBO(s2)
	dli	t1, ~(8192-1)
	and	a2, s1, t1
	or	a3, a2, a1  /* a1 now has entryhi */

	.set	noreorder
	dmtc0	a3, C0_ENTRYHI
	nop
	tlbp
	andi	t0, s1, 4096
	mfc0	t2, C0_INDEX
	bltz	t2, 2f /* test in or out of tlb */
	nop
	tlbr
	beq	t0, zero, 3f /* test odd or even */
	nop
	dmtc0	a0, C0_ENTRYLO1
	b	4f
	nop	
3:	dmtc0	a0,  C0_ENTRYLO0
	nop
4:	tlbwi
	j	other_excpt_ret
	nop




2:	/* not in tlb, so do write random */

	beq	t0, zero, 3f /* test if odd or even */
	nop
	dmtc0	zero,  C0_ENTRYLO0
	dmtc0	a0, C0_ENTRYLO1	
	nop
	tlbwr
	j	other_excpt_ret
	nop

3:	dmtc0	a0,  C0_ENTRYLO0
	dmtc0	zero,  C0_ENTRYLO1
	nop
	tlbwr
	j	other_excpt_ret
	nop	
	
	.set	reorder	

1:	/* page fault either failed or timed out*/
	/* break off ipc with and RECPAGEFAULT TIMEOUT */
	
	/* unwind sender stack to simple syscall */
	daddiu	sp, s2, 1 - 24

	dli	v0, L4_IPC_RERCVPFTO
	move	t8, s2 /* set stcb */
	move	t9, s3 /* set dtcb */
	ld	v1, T_MYSELF-TCBO(t8)

	b	send_only_short
	
END(window_fault_ret)
	.rdata
kern_exc_msg:
	.asciiz "L4 PANIC: L4 kernel exception"	
msg_exc_tr:
	.asciiz "Trap exception"	
PROC(exc_adel)
	.globl	exc_ades
exc_ades:
	.globl	exc_ibe
exc_ibe:
	.globl	exc_dbe
exc_dbe:
	.globl	exc_bp
exc_bp:
	.globl  exc_ri
exc_ri:
	.globl	exc_ov
exc_ov:
	.globl	exc_fpe
exc_fpe:

	.set	noreorder
	/* kernel exceptions handled differently */
	lbu	t0, 240(sp)
	andi	t0, t0, ST_KSU
	beq     t0, zero, 3f  
	move	t2, sp

exc_user:	
	dla	t3, other_excpt_ret
	dli	a2, L4_IPC_NEVER	

	mfc0	t0, C0_STATUS
	li	t1, ST_IE
	or	t1, t0, t1
	mtc0	t1, C0_STATUS
	
	daddiu	sp, sp, -24 
	sb	t0, (sp)
	sd	t3, 8(sp)
	sd	t2, 16(sp)

	/* send cause, epc, badvaddr to exception thread */
	move	s0, s4  
	move	s1, s2
	move	s2, s3

	tcbtop(t8)
	dli	a0, L4_IPC_SHORT_MSG
	dli	a1, L4_IPC_SHORT_MSG
	ld	a4, T_EXCPT_TID-TCBO(t8)
	move	a5, a4
	mtc0	t0, C0_STATUS
	dli	a6, 0

	j	k_ipc
	move	a5, a4

	.set	reorder

3:
	dla	a0, kern_exc_msg
	j	panic	/* ok */
END(exc_adel)


PROC(exc_cpu)
	.set	noreorder
	.set	noat
	/* kernel exceptions handled differently */
	lbu	t0, 240(sp)
	andi	t0, t0, ST_KSU
	beq     t0, zero, 3f  
	move	t2, sp

	/* check if fp unit */
	li	t0, CA_CE_MASK
	and	t1, s4, t0
	li	t0, CA_CE_FP
	bne	t1, t0, exc_user
	nop

	/* enable fp so kernel can manipulate */
	mfc0	t0, C0_STATUS
	li	t1, ST_CU1
	or	t0, t0, t1
	mtc0	t0, C0_STATUS
	
	/* check if current thread owns fp */
	

	lui	a0, KERNEL_BASE
	tcbtop(t8)
	ld	a1, K_FP_THREAD(a0)
	ld	a2, T_MYSELF-TCBO(t8)
	beq	a2, a1, 2f /* yes - enable and return */
	nop

	/* no - save and restore */

	/* save:  check if task still owner */
	tid2tcb(a1,a3)
	ld	a4, T_MYSELF(a3)
	bne	a4, a1, 1f /* no - skip save */
	nop
	/* yes do save */
	cfc1	t2, $31
	nop
#ifdef FLOAT64
#define FLOAD   ldc1
#define FSTORE  sdc1
#else
#define FLOAD   lwc1
#define FSTORE  swc1
#endif		
			
	sd	t2, T_FP_CONTROL(a3)
	FSTORE	$f0,T_FP_REGS+0(a3)
	FSTORE	$f1,T_FP_REGS+8(a3)
	FSTORE	$f2,T_FP_REGS+16(a3)
	FSTORE	$f3,T_FP_REGS+24(a3)
	FSTORE	$f4,T_FP_REGS+32(a3)
	FSTORE	$f5,T_FP_REGS+40(a3)
	FSTORE	$f6,T_FP_REGS+48(a3)
	FSTORE	$f7,T_FP_REGS+56(a3)
	FSTORE	$f8,T_FP_REGS+64(a3)
	FSTORE	$f9,T_FP_REGS+72(a3)
	FSTORE	$f10,T_FP_REGS+80(a3)
	FSTORE	$f11,T_FP_REGS+88(a3)
	FSTORE	$f12,T_FP_REGS+96(a3)
	FSTORE	$f13,T_FP_REGS+104(a3)
	FSTORE	$f14,T_FP_REGS+112(a3)
	FSTORE	$f15,T_FP_REGS+120(a3)
	FSTORE	$f16,T_FP_REGS+128(a3)
	FSTORE	$f17,T_FP_REGS+136(a3)
	FSTORE	$f18,T_FP_REGS+144(a3)
	FSTORE	$f19,T_FP_REGS+152(a3)
	FSTORE	$f20,T_FP_REGS+160(a3)
	FSTORE	$f21,T_FP_REGS+168(a3)
	FSTORE	$f22,T_FP_REGS+176(a3)
	FSTORE	$f23,T_FP_REGS+184(a3)
	FSTORE	$f24,T_FP_REGS+192(a3)
	FSTORE	$f25,T_FP_REGS+200(a3)
	FSTORE	$f26,T_FP_REGS+208(a3)
	FSTORE	$f27,T_FP_REGS+216(a3)
	FSTORE	$f28,T_FP_REGS+224(a3)
	FSTORE	$f29,T_FP_REGS+232(a3)
	FSTORE	$f30,T_FP_REGS+240(a3)
	FSTORE	$f31,T_FP_REGS+248(a3)
	


	/* do restore */
1:	
	FLOAD	$f0,T_FP_REGS+0(a3)
	FLOAD	$f1,T_FP_REGS+8(a3)
	FLOAD	$f2,T_FP_REGS+16(a3)
	FLOAD	$f3,T_FP_REGS+24(a3)
	FLOAD	$f4,T_FP_REGS+32(a3)
	FLOAD	$f5,T_FP_REGS+40(a3)
	FLOAD	$f6,T_FP_REGS+48(a3)
	FLOAD	$f7,T_FP_REGS+56(a3)
	FLOAD	$f8,T_FP_REGS+64(a3)
	FLOAD	$f9,T_FP_REGS+72(a3)
	FLOAD	$f10,T_FP_REGS+80(a3)
	FLOAD	$f11,T_FP_REGS+88(a3)
	FLOAD	$f12,T_FP_REGS+96(a3)
	FLOAD	$f13,T_FP_REGS+104(a3)
	FLOAD	$f14,T_FP_REGS+112(a3)
	FLOAD	$f15,T_FP_REGS+120(a3)
	FLOAD	$f16,T_FP_REGS+128(a3)
	FLOAD	$f17,T_FP_REGS+136(a3)
	FLOAD	$f18,T_FP_REGS+144(a3)
	FLOAD	$f19,T_FP_REGS+152(a3)
	FLOAD	$f20,T_FP_REGS+160(a3)
	FLOAD	$f21,T_FP_REGS+168(a3)
	FLOAD	$f22,T_FP_REGS+176(a3)
	FLOAD	$f23,T_FP_REGS+184(a3)
	FLOAD	$f24,T_FP_REGS+192(a3)
	FLOAD	$f25,T_FP_REGS+200(a3)
	FLOAD	$f26,T_FP_REGS+208(a3)
	FLOAD	$f27,T_FP_REGS+216(a3)
	FLOAD	$f28,T_FP_REGS+224(a3)
	FLOAD	$f29,T_FP_REGS+232(a3)
	FLOAD	$f30,T_FP_REGS+240(a3)
	FLOAD	$f31,T_FP_REGS+248(a3)
	ld	t2, T_FP_CONTROL(a3)
	ctc1	t2, $31


	sd	a2, K_FP_THREAD(a0)
		

2:	/* return from exception, this version of exc_ret enables fp unit */	
	.set	noat
	ld	t0, (sp) /* HI */
	mthi	t0
	ld	t0, 8(sp) /* LO */
	mtlo	t0
	ld	ra, 16(sp)
	ld	s8, 24(sp)
	ld	gp, 32(sp)
	ld	t9, 40(sp)
	ld	t8, 48(sp)
	ld	s7, 56(sp)
	ld	s6, 64(sp)
	ld	s5, 72(sp)
	ld	s4, 80(sp)
	ld	s3, 88(sp)
	ld	s2, 96(sp)
	ld	s1, 104(sp)
	ld	s0, 112(sp)
	ld	t3, 120(sp)
	ld	t2, 128(sp)
	ld	t1, 136(sp)
	ld	t0, 144(sp)
	ld	a7, 152(sp)
	ld	a6, 160(sp)
	ld	a5, 168(sp)
	ld	a4, 176(sp)
	ld	a3, 184(sp)
	ld	a2, 192(sp)
	ld	a1, 200(sp)
	ld	a0, 208(sp)
	ld	v1, 216(sp)
	ld	v0, 224(sp)
	ld	AT, 232(sp)	

	mfc0	k0, C0_STATUS
	ori	k0, k0, ST_EXL
	mtc0	k0, C0_STATUS
	li	k1, 0xffffff00
	and	k0, k0, k1
	lbu	k1, 240(sp)
	or	k0, k0, k1
	
	ld	k1, 248(sp) /* EPC */
	dmtc0   k1, C0_EPC
	mtc0	k0, C0_STATUS
	ld	sp, 256(sp)
	eret

	.set at
	.set reorder
	
		

3:      /* kernel exception -> panic */
	dla	a0, kern_exc_msg
	j	panic	/* ok */
END(exc_cpu)



	
.data
__assert_buf:
	.space 1024
__assert_fmt:
	.asciiz "assert(%s) failed, file '%s', line %d.\r\n"

PROC(exc_tr)
        tcbtop(t0)
        ld      t1, ST_EPC + 1 (t0)
        daddiu  t1, t1, 4
        sd      t1, ST_EPC + 1 (t0)
	move	a4, a2
	move	a3, a1
	move	a2, a0
	dla	a1, __assert_fmt
	dla	a0, __assert_buf
        jal	sprintf
	dla	a0, __assert_buf
        jal     panic_reg_stack  /* FIXME!!! */
        j       other_excpt_ret
END(exc_tr)





PROC(exc_not_defined)
	dla	a0, msg_exc_not_defined
	j	panic /* FIXME!!! */
END(exc_not_defined)

#ifdef L4_IPC_RESTRICT
	.data
illegal_ipc_msg:		
	.asciiz "illegal IPC"
#endif		

PROC(k_ipc)
	/* define a few symbolic names for registers */
	/* incoming, note: message is in s0-s7 */
#define	sdesc  a0
#define	rdesc  a1
/* #define	stime  a2  */
#define timeout a2
/* #define	rtime  a3 */
#define	dthrd  a4
#define wfor   a5
#define vsend  a6
	
	/* locals */
#define	stcb  t8
#define	dtcb  t9
	trace(kipc)
	cbreak 
	/* tcbtop(stcb) done in gen_exc*/ 
	
	/* check receive_only */
	.set	noreorder
	bltz	sdesc, receive_only

	tid2tcb(dthrd, dtcb)
	
	/* trace(dipc)	*/

	ld	t0, T_MYSELF(dtcb)
	ld	v1, T_MYSELF-TCBO(stcb)
	lw	t3, T_FINE_STATE(dtcb)

#ifdef L4_IPC_RESTRICT
	move	v0, zero /* set return code for receiver if all 
			goes well */	
	/* check if ipc is to chief */
	dsll	t1, t0, 32
	xor	t1, t1, v1
	dsll	t1, t1, 4
	dsrl	t1, t1, 53
	beq	t1, zero, return_to_chief1

	/* check if ipc if from chief */

	dsll	t1, v1, 32
	xor	t1, t1, t0
	dsll	t1, t1, 4
	dsrl	t1, t1, 53
	beq	t1, zero, return_to_chief1
	
	/* check if ipc is intra-task */
	xor	t1, v1, t0
	dsll	t1, t1, 36
	dsrl	t1, t1, 53
	beq	t1, zero, return_to_chief1
	nop

	dla	a0, illegal_ipc_msg
	j	panic		/* FIXME: change this to redirection */
	nop
#else	
	/* check for "to_chief" */

	xor	t1, t0, v1
	dsll	t1, 4
	dsrl	t1, 53
	bne	t1, zero, to_chief
	move	v0, zero /* set return code for receiver if all 
			goes well */
#endif	

	/* check if sending to invalid dest */
	.globl return_to_chief1
return_to_chief1:
	bne	t0, dthrd, invalid_dest
	
return_to_chief2:
	/* check if waiting */
	andi	t3, t3, FS_WAIT
	/* check long ipc or ipc_deceit */
	bne	sdesc, zero, ipc_long
	nop
	beq	t3, zero, pending 
	
	ld	t2, T_WFOR(dtcb)

	/* check if open wait */
	beq	t2, zero, deliver
	nop
	/* check if okay closed wait */
	bne	v1, t2, pending
	nop
	/* okay we are doing delivery */
deliver:
/****************************************************************************
 * entry point for completion of some ipc operations, expect rdesc, wfor,
 * v0 (completion code), v1 (sender id), timeout, s0-s7, dtcb, stcb to be 
 * valid
 ****************************************************************************/
	trace(delv)
	cbreak 
	/* check if send_only */
	bltz	rdesc, send_only_short


	/* mark receiver as busy */	
	ori	t0, zero, FS_BUSY
	sw	t0, T_FINE_STATE(dtcb)

	/* check if sender if open wait */
	bne	wfor, zero, 1f
	lui	t1, KERNEL_BASE
	.set	reorder
		/* check if anything pending for sender */
		ld	t0, T_SNDQ_START-TCBO(stcb)
		beq	t0, zero, 1f
			/* arrange sender to restart receiving */
			daddiu	sp, sp, -32
			sd	rdesc, 24(sp)
			sd	wfor,   16(sp)
			sd	timeout, 8(sp)
			dla	t0, sender_restart_receiving
			sd	t0, (sp)
			daddiu	t3, stcb, -TCBO
			ins_busy_list(t3, t1, t0)
			dli	t0, FS_BUSY
			b	3f
	.set	noreorder
1:	
	/* check if timeout never */
	andi	s8, timeout, L4_RCV_EXP_MASK
	beq	s8, zero, 2f
		dli	t0, FS_WAIT
	.set	reorder
		/* zero timeout handled via wakeup list */

		/* set state */
		li	t0, FS_WAIT+FS_WAKEUP

		/* add to wake up list */
		daddiu	t3, stcb, -TCBO
		receive_timeout(timeout, t2, t2)
		ins_wakeup(t2, t3, t1)
2:
	sd	rdesc,	T_RECV_DESC-TCBO(stcb)
	sw	timeout, T_TIMEOUT-TCBO(stcb)
	sd	wfor, T_WFOR-TCBO(stcb)
3:	
	sw	t0, T_FINE_STATE-TCBO(stcb)
		
	trace(sipc)
	thread_switch_fast(stcb, dtcb, t1)
	/* v0 return code set above or in ipc_long or in pending_restart*/
	/* sending thread in v1 */
	trace(eipc)
	cbreak 
	syscall_ret 
END(k_ipc)

PROC(to_chief)
	.set	noreorder
	/* v1 has my tid */
	/* t0 has dest tid */
	
	/* check if directed to chief */
	dsll	t1, t0, 32
	xor	t1, t1, v1
	dsll	t1, t1, 4
	dsrl	t1, t1, 53
	beq	t1, zero, return_to_chief1

	/* check if directed from chief */ 
	dsll	t1, v1, 32
	xor	t1, t1, t0
	dsll	t1, t1, 4
	dsrl	t1, t1, 53
	beq	t1, zero, return_to_chief1
	

	/* okay, message will be redirected, call nchief */
	/* check if dest valid */
	move	s8,v1
	beq	t0, zero, invalid_dest
	/* nchief should work now */
	move	t2, v1
	jal	ipc_nchief
	move	v1, t0

	tid2tcb(v1,dtcb)
	lw	t3, T_FINE_STATE(dtcb)
	xori	v0, v0, L4_IPC_SRC_MASK /* invert inner/outer */
	b	return_to_chief2
	move	v1, s8
END(to_chief)


PROC(sender_restart_receiving)
/* hmmm this bit of code is ugly - it gets setup to be called during
 * send/receive when there is a pending delivery for sender. However
 * pending message to sender can be removed by a kill task, so we need to
 * check everything again, so we might as well call receive_only.
 */
	trace(srrv)
	ld	rdesc, 24(sp)
	ld	wfor, 16(sp)
	ld	timeout, 8(sp)
	daddiu	sp, sp, 32
	tcbtop(stcb)
	b	receive_only
	
END(sender_restart_receiving)

PROC(receive_wakeup)
	trace(rvwu)
	tcbtop(t1)
	daddiu	sp, sp, 8
	dli	v0, L4_IPC_RETIMEOUT
	li	t0, FS_BUSY
	sw	t0, T_FINE_STATE-TCBO(t1)
	syscall_ret 
END(receive_wakeup)
	

PROC(receive_only)
	trace(rvoy)
	cbreak 
	/* check if pending */
	ld	t0, T_SNDQ_START-TCBO(stcb)
	bne	t0, zero, pending_recv_only

	/* check if zero timeout */
leave_waiting:	
	dli	t0, FS_WAIT
	lui	t1, KERNEL_BASE

	andi	t3, timeout, L4_RCV_EXP_MASK
	beq	t3, zero, 2f

	receive_timeout(timeout, t3, t9)
	
	bne	t9, zero, 1f

	/* receive_only with timeout zero means interrupt manipulation
	intended */

	/* check if interrupt */
	dsrl	t0, wfor, 3
	bne	t0, zero, 3f

	/* disassociate any current association of this thread */
	sd	zero, T_INTERRUPT_MASK-TCBO(stcb)
	ld	t0, T_MYSELF-TCBO(stcb)
	ld	t2, K_INT0_THREAD(t1)
	bne	t2, t0, 5f
	sd	zero,  K_INT0_THREAD(t1)
	b	4f
	
5:	ld	t2, K_INT1_THREAD(t1)
	bne	t2, t0, 5f
	sd	zero,  K_INT1_THREAD(t1)
	b	4f

5:	ld	t2, K_INT2_THREAD(t1)
	bne	t2, t0, 5f
	sd	zero,  K_INT2_THREAD(t1)
	b	4f

5:	ld	t2, K_INT3_THREAD(t1)
	bne	t2, t0, 5f
	sd	zero,  K_INT3_THREAD(t1)
	b	4f

5:	ld	t2, K_INT4_THREAD(t1)
	bne	t2, t0, 4f
	sd	zero,  K_INT4_THREAD(t1)


4:	/* check if any new association required */
	beq	wfor, zero, 3f			

	daddiu	s0, wfor, -1
	bne	s0, zero, 5f
	ld	s1, K_INT0_THREAD(t1)
	bne	s1, zero, 6f
	dli	t3, ST_IM2
	sd	t3, T_INTERRUPT_MASK-TCBO(stcb)
	dli	t2, INT0_TCB_BASE & (~(TCB_SIZE-1))
	
	/* inherit scheduling values from int handler */
	lbu	t3, T_TSP-TCBO(stcb)  
	sb	t3, T_TSP(t2)
	sb	t3, T_CTSP(t2)
	lhu	t3, T_TIMESLICE-TCBO(stcb)
	sh	t3, T_TIMESLICE(t2)
	sh	t3, T_REM_TIMESLICE(t2)
	sd	t0, K_INT0_THREAD(t1)
	b	3f

	
5:	daddiu	s0, s0, -1
	bne	s0, zero, 5f
	ld	s1, K_INT1_THREAD(t1)
	bne	s1, zero, 6f
	dli	t3, ST_IM3
	sd	t3, T_INTERRUPT_MASK-TCBO(stcb)
	dli	t2, INT1_TCB_BASE & (~(TCB_SIZE-1))
	
	/* inherit scheduling values from int handler */
	lbu	t3, T_TSP-TCBO(stcb)  
	sb	t3, T_TSP(t2)
	sb	t3, T_CTSP(t2)
	lhu	t3, T_TIMESLICE-TCBO(stcb)
	sh	t3, T_TIMESLICE(t2)
	sh	t3, T_REM_TIMESLICE(t2)
	sd	t0, K_INT1_THREAD(t1)
	b	3f

5:	daddiu	s0, s0, -1
	bne	s0, zero, 5f
	ld	s1, K_INT2_THREAD(t1)
	bne	s1, zero, 6f
	dli	t3, ST_IM4
	sd	t3, T_INTERRUPT_MASK-TCBO(stcb)
	dli	t2, INT2_TCB_BASE & (~(TCB_SIZE-1))

	/* inherit scheduling values from int handler */
	lbu	t3, T_TSP-TCBO(stcb)  
	sb	t3, T_TSP(t2)
	sb	t3, T_CTSP(t2)
	lhu	t3, T_TIMESLICE-TCBO(stcb)
	sh	t3, T_TIMESLICE(t2)
	sh	t3, T_REM_TIMESLICE(t2)
	sd	t0, K_INT2_THREAD(t1)
	b	3f

5:	daddiu	s0, s0, -1
	bne	s0, zero, 5f
	ld	s1, K_INT3_THREAD(t1)
	bne	s1, zero, 6f
	dli	t3, ST_IM5
	sd	t3, T_INTERRUPT_MASK-TCBO(stcb)
	dli	t2, INT3_TCB_BASE & (~(TCB_SIZE-1))

	/* inherit scheduling values from int handler */
	lbu	t3, T_TSP-TCBO(stcb)  
	sb	t3, T_TSP(t2)
	sb	t3, T_CTSP(t2)
	lhu	t3, T_TIMESLICE-TCBO(stcb)
	sh	t3, T_TIMESLICE(t2)
	sh	t3, T_REM_TIMESLICE(t2)
	sd	t0, K_INT3_THREAD(t1)
	b	3f

5:	daddiu	s0, s0, -1
	bne	s0, zero, 6f
	ld	s1, K_INT4_THREAD(t1)
	bne	s1, zero, 6f
	dli	t3, ST_IM6
	sd	t3, T_INTERRUPT_MASK-TCBO(stcb)
	dli	t2, INT4_TCB_BASE & (~(TCB_SIZE-1))

	/* inherit scheduling values from int handler */
	lbu	t3, T_TSP-TCBO(stcb)  
	sb	t3, T_TSP(t2)
	sb	t3, T_CTSP(t2)
	lhu	t3, T_TIMESLICE-TCBO(stcb)
	sh	t3, T_TIMESLICE(t2)
	sh	t3, T_REM_TIMESLICE(t2)
	sd	t0, K_INT4_THREAD(t1)
	b	3f
	
	
6:	dli	v0, L4_IPC_ENOT_EXISTENT
	syscall_ret 
		

3:	dli	v0, L4_IPC_RETIMEOUT
	syscall_ret 

1:
		/* set state */
		li	t0, FS_WAIT+FS_WAKEUP

		/* add to wake up list */
		daddiu	t3, stcb, -TCBO
		
		ins_wakeup(t9, t3, t1)
2:
	/* enable interrupts if thread associated with interrupt */
	.set	noreorder
	ld	t2, T_INTERRUPT_MASK-TCBO(stcb)
	mfc0	t3, C0_STATUS
	or	t3, t3, t2
	mtc0	t3, C0_STATUS
	.set	reorder

	sw	t0, T_FINE_STATE-TCBO(stcb)
	sd	rdesc,	T_RECV_DESC-TCBO(stcb)
	sw	timeout, T_TIMEOUT-TCBO(stcb)
	sd	wfor, T_WFOR-TCBO(stcb)
	trace(erco)
       	to_next_thread(t1)
END(receive_only)

PROC(pending_recv_only)
	trace(pery)
	/* check if open wait */
	beq	wfor, zero,  1f

	/* FIXME: check if wfor interrupt */

	/* check if wfor in pending q */
3:	ld	t1, T_MYSELF(t0)	/* I think this is wrong, we should */
	beq	t1, wfor, 1f		/* check the vsend if deceited - GH */
	ld	t0, T_SNDQ_NEXT(t0)
	bne	t0, zero, 3b
	/* okay nothing wfor in pending queue */	

	j	leave_waiting	

1:	/* deliver thread in t0  */
	
	/* first store receiver state for sender to find */
	sd	rdesc,	T_RECV_DESC-TCBO(stcb)
	sw	timeout,  T_TIMEOUT-TCBO(stcb)
	lui	t1, KERNEL_BASE

	thread_switch_fast(stcb, t0, t1)

	ld	ra, (sp)
	jr	ra
END(pending_recv_only)

PROC(send_only_short)
	trace(sosh)
	cbreak 
	.set	noreorder
	daddiu	sp, sp , -16
	dla	t0, send_only_short_restart
	sd	t0, (sp)
	andi	t1, v0, L4_IPC_ERROR_MASK
	beq	t1, zero, 1f
	sd	zero, 8(sp)
	
	ori	t1, v0, L4_IPC_SND_ERR_MASK
	sd	t1, 8(sp)

1:	dli	t0, FS_BUSY /* unlock sender on error in do_long_ipc */
	sw	t0,  T_FINE_STATE-TCBO(stcb)
	/* mark receiver as busy */	
	ori	t0, zero, FS_BUSY
	sw	t0, T_FINE_STATE(dtcb)
	daddiu	t3, stcb, -TCBO
	lui	t1, KERNEL_BASE
	.set	reorder
		
	ins_busy_list(t3, t1, t0)

	/* thread switch */
	thread_switch_fast(stcb, dtcb, t1)

	/* do receiver return code */
	/* v0 return code should be set already, see deliver for how */
	/* sending thread still in v1 */

	syscall_ret 
END(send_only_short)


PROC(send_only_short_restart)
	trace(sosr)
	cbreak 
	/* do sender return code */
	ld	v0, 8(sp)
	daddiu	sp, sp , 16
	syscall_ret 
END(send_only_short_restart)

PROC(invalid_dest)
	dli	v0, L4_IPC_ENOT_EXISTENT
	syscall_ret 
END(invalid_dest)

PROC(pending)
	/* get here if sending and destination isn't receiving yet */
	trace(pend)
	cbreak 
	/* v1 has my thread id */
	andi	t2, timeout, L4_SND_EXP_MASK
	dsrl	t2, t2, 4
	beq	t2, zero, 1f
	send_timeout(timeout, t2, s8)
	bne	s8, zero, 1f
	dli	v0, L4_IPC_SETIMEOUT
	syscall_ret 
1:
	/* THINK:	 v1 can be deceited */
	bne	dthrd, v1, 1f
	dli	v0,  L4_IPC_ENOT_EXISTENT
	syscall_ret 
1:
	dli	t0, FS_POLL
	lui	t1, KERNEL_BASE
	daddiu	t3, stcb, -TCBO

	beq	t2, zero, 2f
		/* set state */
		li	t0, FS_POLL+FS_WAKEUP

		ins_wakeup(s8, t3, t1)
2:
	sw	t0,  T_FINE_STATE-TCBO(stcb)
	
	ins_sendq_end(t3, dtcb)
	
	/* stack my state */
	daddiu	sp,sp,-144

	sd	sdesc, 8(sp)
	sd	rdesc, 16(sp)
	sd	timeout, 24(sp)
	sd	dthrd, 32(sp)
	sd	wfor, 40(sp)
	sd	vsend, 48(sp)
	/* stcb faster via stack macro */
	sd	dtcb, 56(sp)
	sd	dtcb, T_COMM_PARTNER-TCBO(stcb)
	sd	s0, 64(sp)
	sd	s1, 72(sp)
	sd	s2, 80(sp)
	sd      s3, 88(sp)
	sd	s4, 96(sp)
	sd	s5, 104(sp)
	sd	s6, 112(sp)
	sd	s7, 120(sp)
	sd	v1, 128(sp)
	sd	v0, 136(sp)

	/* store restart address */
	dla	t0, pending_restart
	sd	t0, (sp)

	trace(epnd)
	cbreak 
	lui	t0, KERNEL_BASE
	to_next_thread(t0)
	
END(pending)

	.data
ps_msg:
	.asciiz "pending restart\r\n"

PROC(pending_restart)
	trace(pert)
	/* FIXME: handle case when receiver is killed */
	cbreak 
	/* unstack my state */
	ld	sdesc, 8(sp)
	ld	rdesc, 16(sp)
	ld	timeout, 24(sp)
	ld	dthrd, 32(sp)
	ld	wfor, 40(sp)
	ld	vsend, 48(sp)
	tcbtop(stcb)
	ld	dtcb, 56(sp)
	ld	s0, 64(sp)
	ld	s1, 72(sp)
	ld	s2, 80(sp)
	ld      s3, 88(sp)
	ld	s4, 96(sp)
	ld	s5, 104(sp)
	ld	s6, 112(sp)
	ld	s7, 120(sp)
	ld	v1, 128(sp)
	ld	v0, 136(sp)

	/* ignore stacked restart address (sp)*/
	daddiu	sp,sp, 144

	/* dequeue myself for dtcb pending queue */
	daddiu	t3, stcb, -TCBO
	rem_sendq(t3, dtcb, t0)

	/* make myself BUSY */
	dli	t0, FS_BUSY
	sw	t0, T_FINE_STATE-TCBO(stcb)
	
	/* set return code */

	bne	sdesc, zero, ipc_long_deliver

	b	deliver
	
END(pending_restart)

PROC(pending_wakeup)
	trace(pewu)

	/* send timeout, don't bother unstacking all registers */
	ld	dtcb, 56(sp)
	daddiu	sp,sp, 144
	
	/* remove myself from pending queue */
	tcbtop(stcb)
	daddiu	t3, stcb, -TCBO
	rem_sendq(t3, dtcb, t0)

	/* made busy by schedule */

	/* load return code */
	dli	v0, L4_IPC_SETIMEOUT
	syscall_ret 
END(pending_wakeup)

PROC(pending_recv_killed)
	trace(perk)
	/* recv killed by task_new, don't bother unstacking all registers */
	daddiu	sp,sp, 144
	
	/* already removed from pending queue */

	/* made busy by task_new */

	/* load return code */
	dli	v0, L4_IPC_ENOT_EXISTENT
	syscall_ret 
END(pending_recv_killed)

PROC(ipc_long)
	trace(ipcl)

        /* test for deceiving ipc */
	andi	t0, sdesc, L4_IPC_DECEIT_MASK
	beq	t0, zero, 2f
	tid2tcb(vsend,t0)

	/* check vsend valid */
	ld	t0, T_MYSELF(t0)
	beq	t0, zero, invalid_dest
	
	/* nchief vsend */
	move	t3, v0
	move	s8, v1
	move	t2, v1
	move	v1, vsend
	jal	long_ipc_nchief
	move	a3, v0
	
	/* nchief dthrd */
	move	t2, s8
	move	v1, dthrd
	jal	long_ipc_nchief

	/* check okay */
	xor	t0, a3, v0
	andi	t0, t0,  L4_IPC_SRC_MASK
	beq	t0, zero, 3f
	/* change v1, and v0 */

	/* we are deceiving */
	move	v1, vsend
	ori	v0, t3, L4_IPC_DECEIT_MASK
	b	2f
	/* can't deceit, use normal source id */
3:
	move	v0, t3
	move	v1, s8
2:
	/* check if waiting */
	lw	t3, T_FINE_STATE(dtcb)
	andi	t3, t3, FS_WAIT
	beq	t3, zero, pending 
	/* check if open wait */
	ld	t2, T_WFOR(dtcb)
	beq	t2, zero, ipc_long_deliver

	/* check if okay closed wait */
	bne	v1, t2, pending

ipc_long_deliver:
        /* FIXME: avoid C if all we have is deceiving (not long) ipc */
/*	ld	reg, L4_IPC_DECEIT_MASK  */
/*	beq	sdesc, reg, deliver	 */

	/* about to call C so save registers C does not preserve */
	trace(ildl)
	daddiu	sp,sp, -128
	/* sdesc not needed any more */
	sd	rdesc,   (sp)
	sd	timeout,  8(sp)
	sw	timeout, T_TIMEOUT-TCBO(stcb)
	sd	dthrd, 16(sp) /* dthrd is a4 */
	sd	wfor,  24(sp)
	sd	vsend, 32(sp)
	/* stcb faster via stack macro */
	sd	dtcb,  40(sp)
	sd	v1, 48(sp)
	sd	v0, 56(sp)

	/* store s0 registers so C can see them */
	sd	s0, 64(sp)
	sd	s1, 72(sp)
	sd	s2, 80(sp)
	sd	s3, 88(sp)
	sd	s4, 96(sp)
	sd	s5, 104(sp)
	sd	s6, 112(sp)
	sd	s7, 120(sp)

	
	/* now load args to C func */
	/* sdesc already in a0 */
	ld	a1, T_RECV_DESC(dtcb)
	daddiu	a2, sp, 64
	daddiu	a3, stcb, -TCBO
	move	a4, dtcb
	move	a5, v0
	trace(ild2)
	jal	do_long_ipc

	/* now restore what is needed by deliver to finish */
	trace(2ipl)
	cbreak 
	move	a0, v0
	ld	rdesc,   (sp)
	ld	timeout,  8(sp)
	ld	dthrd, 16(sp)
	ld	wfor,  24(sp)
	ld	vsend, 32(sp)
	ld	dtcb,  40(sp)
	ld	v1, 48(sp)
	ld	v0, 56(sp)
	
	daddiu	sp,sp,128

	tcbtop(stcb)
	or 	v0, v0, a0
	andi	t0, a0,  L4_IPC_ERROR_MASK 
	trace(eipl)

	beq	t0, zero, deliver
	/* something failed in long ipc */
	b	send_only_short


END(ipc_long)
