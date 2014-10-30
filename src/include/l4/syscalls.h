#ifndef __L4_SYSCALLS_H__ 
#define __L4_SYSCALLS_H__ 
/****************************************************************************
 * $Id: syscalls.h,v 1.12 1998/12/23 02:03:59 gernot Exp $
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

#include <l4/types.h>

/****************************************************************************
 * system call numbers for assembly hackers
 ****************************************************************************/

#if defined(_LANGUAGE_ASSEMBLY)
#define SYSCALL_IPC              0
#define SYSCALL_FPAGE_UNMAP      1
#define SYSCALL_ID_NEAREST       2
#define SYSCALL_ID_NCHIEF        3
#define SYSCALL_THREAD_SWITCH    4
#define SYSCALL_THREAD_SCHEDULE  5
#define SYSCALL_LTHREAD_EX_REG   6
#define SYSCALL_TASK_CREATE      7
#define MAX_SYSCALL_NUMBER       7
#endif

/****************************************************************************
 * prototypes and constants for system calls other than ipc
 ****************************************************************************/

#ifdef _LANGUAGE_C
extern void l4_fpage_unmap(l4_fpage_t fpage, dword_t map_mask);

/* valid values for mask */
#define L4_FP_REMAP_PAGE        0x00    /* Page is set to read only */
#define L4_FP_FLUSH_PAGE        0x02    /* Page is flushed completely */
#define L4_FP_OTHER_SPACES      0x00    /* Page is flushed in all other */
                                        /* address spaces */
#ifdef __GNUC__
#define L4_FP_ALL_SPACES        0x8000000000000000LL
                                        /* Page is flushed in own address */ 
                                        /* space too */
#else
#define L4_FP_ALL_SPACES        0x8000000000000000ul
#endif

extern l4_threadid_t l4_myself(void);


extern int l4_id_nearest(l4_threadid_t destination,
                         l4_threadid_t *next_chief);

#endif

/* return values of l4_id_nearest */
#define L4_NC_SAME_CLAN         0x00    /* destination resides within the */
                                        /* same clan */
#define L4_NC_INNER_CLAN        0x0C    /* destination is in an inner clan */
#define L4_NC_OUTER_CLAN        0x04    /* destination is outside the */
                                        /* invoker's clan */

#if defined(_LANGUAGE_C)
extern void l4_thread_ex_regs(l4_threadid_t destination,
                              dword_t eip, dword_t esp,
                              l4_threadid_t *excpt, l4_threadid_t *pager,
                              dword_t *old_eip, dword_t *old_esp);

extern void l4_thread_switch(l4_threadid_t destination);

extern cpu_time_t
l4_thread_schedule(l4_threadid_t dest, l4_sched_param_t param,
                   l4_threadid_t *ext_preempter, l4_threadid_t *partner,
                   l4_sched_param_t *old_param,
                   l4_threadid_t *old_ext_preempter);

extern l4_taskid_t 
l4_task_new(l4_taskid_t destination, dword_t mcp_or_new_chief, 
            dword_t esp, dword_t eip, l4_threadid_t pager,
            l4_threadid_t excpt);
#endif
#endif
