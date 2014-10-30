/****************************************************************************
 * $Id: serial_test3.c,v 1.7 1998/12/28 12:23:20 gernot Exp $
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
#include <libc.h>
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>

#define STACK_SIZE 1024
dword_t _stack[STACK_SIZE];
dword_t *_sp = &_stack[STACK_SIZE-1];

static const l4_threadid_t SERIAL_TID = {0x1002000000060001L};
#ifdef P4000
static const l4_threadid_t SONIC_TID = {0x1002000000080001L};
#endif

void main()
{
  int r;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  char *c;
  l4_threadid_t tid;
  long count;
  dword_t clock;
#ifndef P4000
  l4_kernel_info *kip;
#endif

  count = 0;
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "My thread id is 0x%lx\r\n",tid.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
 

#ifndef P4000
  msg.reg[0] = SIGMA0_KERNEL_INFO_MAP;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
			 (void *) ((dword_t) L4_IPC_SHORT_FPAGE |
				   (L4_WHOLE_ADDRESS_SPACE << 2 )),
			 &msg, L4_IPC_NEVER, &result);
  kip = (l4_kernel_info *)msg.reg[0];
  if (r) {
    sprintf(c, "Mapping KIP failed: 0x%lx\n", result.msgdope);
  } else {
    sprintf(c, "Mapped KIP at 0x%lx\n", (dword_t)kip);
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
#endif

  while (1)
  {
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,2,6,0,0),&result);

#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_SHORT_MSG,
			 &msg, L4_IPC_NEVER, &result);
    assert(r == 0);
    clock = msg.reg[0];
#else
    clock = kip->clock;
#endif

    count++;
    sprintf(c,"count = %ld, time = %lu\r\n", count, clock);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		  L4_IPC_NEVER, &result);

    
    assert(r == 0);

  }
}
