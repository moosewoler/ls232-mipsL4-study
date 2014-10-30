#ifndef CHIEF_IPC_H
#define CHIEF_IPC_H

#include <stdio.h>
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>

#define l4_send_task_short_f(dest, buffer, result, fstring...) \
 (sprintf((char *) (buffer), ## fstring),  \
  l4_mips_ipc_send((dest), L4_IPC_SHORT_MSG, (buffer), L4_IPC_NEVER, (result)))

#define l4_send_task_short(dest, result, string) \
   l4_mips_ipc_send((dest), L4_IPC_SHORT_MSG, (l4_ipc_reg_msg_t *) string, \
		    L4_IPC_NEVER, (result))


void chief_IPC_test(int childtask, l4_threadid_t pager_thread);

#endif
