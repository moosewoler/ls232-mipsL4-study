/****************************************************************************
 * $Id: ipc_test_server.c,v 1.8 1998/12/28 12:23:14 gernot Exp $
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

static const l4_threadid_t SERIAL_TID = {0x1002000000060001};

#define STACK_SIZE 512
unsigned long _stack[STACK_SIZE];
unsigned long *_sp = &_stack[STACK_SIZE -1];

static dword_t stack[128];

void mem_server_loop(void);

void mem_thread(void)
{
  int r,i;
  l4_msgdope_t result;
  unsigned long dummy;
  l4_threadid_t tid;
  char *c;
  l4_ipc_reg_msg_t msg;
  
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "mem test server, my thread id is 0x%lx\r\n",tid.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  c = (char *) &msg.reg[0];
  sprintf(c, "mapping 1000 pages and starting server loop\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  for (i = 0; i < 1000; i++)
  {
    dummy = *(volatile unsigned long *) (8l * 1024 * 1024 + i *4096);
  }
  mem_server_loop();
}

void server_loop(l4_threadid_t);

void main()
{
  int r;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  char *c;
  l4_threadid_t tid, pre , page ;
  l4_threadid_t mem_tid= {1<<10};
  long count;
  count = 0;
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "IPC test server, my thread id is 0x%lx\r\n",tid.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  c = (char *) &msg.reg[0];
  sprintf(c, "ex reg memory server\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  pre.ID = -1;
  page.ID = -1;
  l4_thread_ex_regs(mem_tid,
		    (dword_t) mem_thread,
		    (dword_t) &stack[127],
		    &pre,
		    &page,
		    &count,
		    &count);
  for ( c = (char *) 0x7f000; (dword_t)c < 0x81000; c += 4096)
  {
    r = *(volatile char *) c;
  }
  

  r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG,
		       &msg,  L4_IPC_NEVER, &result);
  assert(r == 0);

  server_loop(tid);
}

