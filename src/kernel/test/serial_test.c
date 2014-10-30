/****************************************************************************
 * $Id: serial_test.c,v 1.7 1998/12/28 12:23:19 gernot Exp $
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
#include <l4/ipc.h>
#include <l4/syscalls.h>

#define STACK_SIZE 1024
dword_t _stack[STACK_SIZE];
dword_t *_sp = &_stack[STACK_SIZE-1];

static const l4_threadid_t SERIAL_TID = {0x1002000000060001L};

void main()
{
  int r;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  char *c;
  l4_threadid_t tid;
  long count;
  count = 0;
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "My thread id is 0x%lx\n",tid.ID);

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  while (1)
  {
    static const l4_threadid_t TEST_TID = {0x260001};
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
    
    r = l4_mips_ipc_send(TEST_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    if (r == L4_IPC_ENOT_EXISTENT) {
      sprintf(c,"r = 0x%lx - PASSED\n", result.msgdope);
    } else {
      sprintf(c,"r = 0x%lx - FAILED\n", result.msgdope);
    }
    
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  }
}

