#ifndef KERNEL_H
#define KERNEL_H
/****************************************************************************
 * $Id: kernel.h,v 1.43 1999/09/30 06:27:29 alanau Exp $
 * Copyright (C) 1997, 1998 Kevin Elphinstone, Univeristy of New South
 * Wales.
 *
 * This file is part of the L4/MIPS micro-kernel distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ****************************************************************************/

#include <kernel/types.h>
#include <l4/types.h>
/****************************************************************************
 * thread control block variables
 ****************************************************************************/


#define T_SNDQ_END      0
#define T_WAKEUP_LINK   8
#define T_BUSY_LINK     16
#define T_INT_LINK      24
#define T_WFOR          32
#define T_SNDQ_START    40
#define T_STACK_POINTER 48
#define T_ASID          56
#define T_GPT_POINTER   64
#define T_MYSELF        72
#define T_FINE_STATE    80
#define T_TIMEOUT       84
#define T_RECV_DESC     88
#define T_PRESENT_NEXT  96
#define T_CHILD_TASK     104
#define T_REM_TIMESLICE 112
#define T_TIMESLICE     114
#define T_MCP           116 /* maximum controlled priority */
#define T_BPAD1         117 /* byte padding: should never be used */
#define T_CTSP          118 /* current time slice priority */
#define T_TSP           119 /* time slice priority */
#define T_WAKEUP        120
#define T_SOON_WAKEUP_LINK   128
#define T_LATE_WAKEUP_LINK   136
#define T_SNDQ_NEXT          144
#define T_SNDQ_PREV          152
#define T_PAGER_TID          160
#define T_EXCPT_TID          168
#define T_COMM_PARTNER       176
#define T_WDW_MAP_ADDR       184       
#define T_INTERRUPT_MASK     192       
#define T_STACKED_FINE_STATE 200
#define T_COARSE_STATE       204
#define T_STACKED_COMM_PRTNR 208
#define T_SISTER_TASK        216
#define T_SISTER_TASK_PREV   224
#define T_FP_REGS            232
#define T_FP_CONTROL         488
#define T_PT_SIZE            496
#define T_PT_NUMBER          504
#define T_CPU_TIME           512

#if defined(_LANGUAGE_C)

typedef struct tcb {
  struct tcb *sndq_end;
  struct tcb *wakeup_link;
  struct tcb *busy_link;
  struct tcb *int_link;
  unsigned long wfor;
  struct tcb *sndq_start;
  unsigned long stack_pointer;
  udw_t asid;
  void *gpt_pointer;
  unsigned long myself;
  unsigned int fine_state;
  unsigned int timeout;
  long recv_desc;
  struct tcb *present_next;
  struct tcb *child_task;
  uhw_t rem_timeslice;
  uhw_t timeslice;
  ub_t mcp; /* maximum controlled priority */
  ub_t bpad1; /* byte padding: should never be accessed */
  ub_t ctsp; /* current time slice priority */
  ub_t tsp; /* time slice priority */
  long wakeup;
  struct tcb *soon_wakeup_link;
  struct tcb *late_wakeup_link;
  struct tcb *sndq_next;
  struct tcb *sndq_prev;
  udw_t pager_tid;
  udw_t excpt_tid;
  struct tcb *comm_partner;
  unsigned long wdw_map_addr;
  unsigned long interrupt_mask;
  unsigned int stacked_fine_state;
  unsigned int coarse_state;
  struct tcb *stacked_comm_prtnr;
  struct tcb *sister_task;
  struct tcb *sister_task_prev;
  udw_t fp_regs[32];
  udw_t fp_control;
  udw_t pt_size;
  udw_t pt_number;
  udw_t cpu_time;
} tcb_t;


#endif
/* list consts */

#if defined(_LANGUAGE_ASSEMBLY)
#define END_LIST       -1
#define OUT_LIST       0
#else
#define END_LIST       ((tcb_t *)-1l)
#define OUT_LIST       ((tcb_t *)0)
#endif

/* thread states */
#define FS_WAIT        1
#define FS_BUSY        2
#define FS_WAKEUP      4
#define FS_POLL        8
#define FS_LOCKS       16
#define FS_INACTIVE    32
#define FS_DYING       64
#define FS_LOCKR       128

/* tcb states */
#define CS_UNUSED_TCB  0
#define CS_USED_TCB    1
#define CS_INVALID_TCB 2

/* tid table states */

#define TT_VERSION_MASK          037777    /* 14 bits */
#define TT_OVRFLW_MASK           040000    /* 15th bit */
#define TT_CHIEF_MASK       01777400000    /* 18-28  11 bits */
#define TT_INACTIVE_MASK   020000000000    /* bit 32 */

#define TID_TASK_MASK       01777400000
#define TID_TASK_SHIFT      17
#define TID_THREAD_MASK     00000376000
#define TID_THREAD_SHIFT    10

/* scheduling constants */

#define MAX_TIMESLICE 32
#define MAX_PRIORITY 255

/* wake list consts */

#define SOON_TIME  16
#define LATE_TIME  1024

/****************************************************************************
 * kernel variables 
 ****************************************************************************/

#define MAX_TASKS  2048
#define MAX_THREADS 128

#define TCB_SIZE                0x800
#define TCB_SIZE_SHIFT          11
#define TCBO                    (TCB_SIZE - 1)

#define TASK_TCB_SIZE_SHIFT     18

#define INITIAL_TIMER_TICKS     49927 /* per ms (hand tuned)  */
#define TIMER_TICKS             49999
#define LEN_TIME_SLICE          32

/* memory map */
/*      TLB2_BASE               0x8002 */
/*      KERNEL_BASE             0x8004 = 0xffff ffff 8004 0000 via lui */

#ifdef INDY

# define TID_TABLE_BASE          0xffffffff88041000 
# define DEBUGGER_STACK        /*0x08043000 */  0xffffffff88046ff8
# define SIGMA0_TCB_BASE         0x08047000
# define NIL_TCB_BASE            0x08048000
# define INVALID_TCB_BASE        0x08049000
# define ASID_BASE               0xffffffff8804a000
# define INT0_TCB_BASE           0xffffffff8804b7f8
# define INT1_TCB_BASE           0xffffffff8804bff8
# define INT2_TCB_BASE           0xffffffff8804c7f8
# define INT3_TCB_BASE           0xffffffff8804cff8
# define INT4_TCB_BASE           0xffffffff8804d7f8
# define KERNEL_INFO_PAGE        0x0804f000
# define INDY_OFFSET             0x08000000
#elif defined(U4600) || defined(P4000)

# define TID_TABLE_BASE          0xffffffff80041000 
# define DEBUGGER_STACK        /*0x00043000 */  0xffffffff80046ff8
# define SIGMA0_TCB_BASE         0x00047000
# define NIL_TCB_BASE            0x00048000
# define INVALID_TCB_BASE        0x00049000
# define ASID_BASE               0xffffffff8004a000
# define INT0_TCB_BASE           0xffffffff8004b7f8
# define INT1_TCB_BASE           0xffffffff8004bff8
# define INT2_TCB_BASE           0xffffffff8004c7f8
# define INT3_TCB_BASE           0xffffffff8004cff8
# define INT4_TCB_BASE           0xffffffff8004d7f8
# define KERNEL_INFO_PAGE        0x0004f000
# define INDY_OFFSET             0x0
#elif defined(SIM)

#define INVALID_TCB_BASE         0x0
#define NIL_TCB_BASE             0x0

#endif

#define PROFILE_START_ADDR  (0xffffffff80000000 + 63*1024*1024)
#define PROFILE_END_ADDR    (0xffffffff80000000 + 64*1024*1024)

#define TCB_VBASE                               0xc000000000000000
#define RECV_WINDOW_BASE                        0x4000000000000000
#define RECV_WINDOW_SIZE                        (16*1024*1024)
#define RECV_WINDOW_SHIFT                       24
#define USER_ADDR_TOP                           0x0000010000000000
#define KERNEL_MEM_FRAC          10   /* fraction of mem reserved by
					 kernel at top of RAM */

#if defined(_LANGUAGE_ASSEMBLY)

#ifdef INDY

# define TLB2_BASE               0x8802
# define TLB2_BASE64             0xffffffff88020000
# define KERNEL_BASE             0x8804 /* = 0xffff ffff 8804 0000 via lui */

#elif defined(U4600) || defined(P4000)

# define TLB2_BASE               0x8002
# define TLB2_BASE64             0xffffffff80020000
# define KERNEL_BASE             0x8004 /* = 0xffff ffff 8804 0000 via lui */

#elif defined(SIM)

# define TLB2_BASE               0x0
# define TLB2_BASE64             0x0

#endif

#define TLB2_MAX_SIZE               (128 * 1024)

/* really, 1 byte less than actual size */
#define BUSY_LIST_SIZE          (MAX_PRIORITY * 8)

#define K_STACK_BOTTOM          0      /* stack loaded when user->kernel */
#define K_S0_SAVE               8
#define K_FREE_ASID_LIST        16  
#define K_WAKEUP_LIST           24     /* counter of timer ticks */
#define K_CLOCK                 32
#define K_PRESENT_LIST          40
#define K_INT_LIST              48
#define K_SOON_WAKEUP_LIST      56
#define K_LATE_WAKEUP_LIST      64
#define K_TIMESLICE            72
#define K_PRIORITY              76
#define K_MEMORY_SIZE           80
#define K_FRAME_TABLE_BASE      88
#define K_FRAME_TABLE_SIZE      96 
#define K_FRAME_TABLE_POINTER   104
#define K_TRACE_REG_SAVE        112
#define K_S1_SAVE               120     
#define K_S2_SAVE               128
#define K_S3_SAVE               136
#define K_S4_SAVE               144
#define K_TLB_T0_SAVE           152      /* space to free extra reg for tlb */
#define K_TLB_T1_SAVE           160
#define K_GPT_POINTER           168      
#define K_TLB_T2_SAVE           176
#define K_TLB_T3_SAVE           184
#define K_PROFILE_ADDR          192
#define K_TCB_GPT_GUARD         200
#define K_TCB_GPT_POINTER       208
#define K_TLB_T8_SAVE           216
#define K_FRAME_LIST            224
#define K_GPT_FREE_LIST         232
#define K_MT_FREE_LIST          240
#define K_SYSCALL_JMP_TABLE     248  /* reserve 8 spaces */
#define K_EXC_JMP_TABLE         312  /* reserve 16 spaces */
#define K_BREAK_ON              440
#define K_SIGZ_TCB              448
#define K_GPT_LEAF_FREE_LIST    456
#define K_ASID_FIFO_COUNT       464
#define K_INT0_THREAD           472
#define K_INT1_THREAD           480
#define K_INT2_THREAD           488
#define K_INT3_THREAD           496
#define K_INT4_THREAD           504
#define K_FP_THREAD             512
#define K_TLB_MISS              520
#define K_TLB_MISS_TIME         528
#define K_TLB2_MISS             536
#define K_PRIO_BUSY_LIST        544 /* array of prio. circular run queues */
#define K_TID_TABLE             0x1000

#else /* assume C */

typedef struct kern {
  dw_t stack_bottom;
  udw_t s0_save;
  udw_t *free_asid_list;
  tcb_t *wakeup_list;
  udw_t clock;
  tcb_t *present_list;
  tcb_t *int_list;
  tcb_t *soon_wakeup_list;
  tcb_t *late_wakeup_list;
  uw_t timeslice;
  uw_t priority;
  udw_t memory_size;
  udw_t *frame_table_base;
  udw_t frame_table_size;
  udw_t frame_table_pointer;
  udw_t trace_reg_save;
  udw_t s1_save;
  udw_t s2_save;
  udw_t s3_save;
  udw_t s4_save;
  udw_t tlb_t0_save;
  udw_t tlb_t1_save;
  void *gpt_pointer;
  udw_t tlb_t2_save;
  udw_t tlb_t3_save;
  udw_t profile_addr;
  udw_t tcb_gpt_guard;
  udw_t tcb_gpt_pointer;
  udw_t tlb_t8_save;
  udw_t frame_list;
  udw_t gpt_free_list;
  udw_t mt_free_list;
  udw_t syscall_jmp_table[8];
  udw_t exc_jmp_table[16];
  udw_t break_on;
  tcb_t *sigz_tcb;
  udw_t gpt_leaf_free_list;
  udw_t asid_fifo_count;
  udw_t int0_thread;
  udw_t int1_thread;
  udw_t int2_thread;
  udw_t int3_thread;
  udw_t int4_thread;
  udw_t fp_thread;
  udw_t tlb_miss;
  udw_t tlb_miss_time;
  udw_t tlb2_miss;
  tcb_t *prio_busy_list[MAX_PRIORITY+1];
} kernel_vars;

# ifdef INDY

#  define KERNEL_BASE            ((kernel_vars *) 0xffffffff88040000)
#  define TLB2_BASE               0xffffffff88020000

# elif  defined(U4600) || defined(P4000)

#  define KERNEL_BASE            ((kernel_vars *) 0xffffffff80040000)
#  define TLB2_BASE               0xffffffff80020000

# elif SIM

extern kernel_vars sim_kern;
#  define KERNEL_BASE (&sim_kern)
#  define TLB2_BASE tlb2_base_ptr
# endif


/* internal prototypes */

void tlb_flush_window(udw_t);
extern hword_t l4_version;
extern hword_t l4_build;

#endif

/* "system" thread ids */
#define SPINNER_TID  (1 << 17) /* purposely wrong */

/* a few helpful contants */

#define ST_SP             -8
#define ST_EPC            -16
#define ST_EX_SIZE        264
#define ST_EX_V0          -40
#define ST_EX_V1          -48



#endif
