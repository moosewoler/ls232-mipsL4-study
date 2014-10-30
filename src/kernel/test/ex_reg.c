/****************************************************************************
 * $Id: ex_reg.c,v 1.6 1998/12/28 12:23:11 gernot Exp $
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

#define STACK_SIZE 1024
dword_t _stack[STACK_SIZE];
dword_t *_sp = &_stack[STACK_SIZE-1];

static const l4_threadid_t SERIAL_TID = {0x1002000000060001L};
static const l4_threadid_t SONIC_TID = {0x1002000000080001L};

dword_t thread_1_stack[128];
dword_t thread_2_stack[128];

void thread_1(void)
{
  l4_msgdope_t result;
  int r;
  l4_ipc_reg_msg_t msg;
  char *c;
  long count;
  l4_threadid_t tid = l4_myself();
  
  count = 0;
  c = (char *) &msg.reg[0];

  sprintf(c,"0x%lx: start of thread\n", tid.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);

    
  while (1)
  {
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
    
    count++;
    sprintf(c,"0x%lx: count = %ld\n", tid.ID, count);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
    assert(r == 0);
  }
}

void main()
{
  l4_msgdope_t result;
  int r;
  l4_ipc_reg_msg_t msg;
  char *c;
  l4_threadid_t tid, pager, preempter;
  long count;
  dword_t old_eip, old_sp;
  l4_threadid_t t4 = {5 << 10};
  l4_threadid_t t2 = {2 << 10};
  


  count = 0;
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "My thread id is 0x%lx\n",tid.ID);

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);

  pager.ID = -1ul;
  preempter.ID = -1ul;
  
  l4_thread_ex_regs(t4, -1 ,-1, &preempter, &pager, &old_eip, &old_sp);

  sprintf(c,"ip=0x%lx\n", old_eip);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);
  
  sprintf(c,"sp=0x%lx\n", old_sp);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);

  pager.ID = -1ul;
  preempter.ID = -1ul;
  l4_thread_ex_regs(t4, (dword_t) thread_1 ,
		    (dword_t) &thread_1_stack[127],
		    &preempter, &pager, &old_eip, &old_sp);
  
  l4_thread_ex_regs(t2, (dword_t) thread_1 ,
		    (dword_t) &thread_2_stack[127],
		    &preempter, &pager, &old_eip, &old_sp);
  
  
  while (1)
  {
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,3,5,0,0),&result);

    count++;
#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
    sprintf(c,"count = %ld, time = %lu\n", count, msg.reg[0]);
#else
    sprintf(c,"count = %ld\n", count);
#endif

    sprintf(c,"count = %ld, time = %lu\n", count, msg.reg[0]);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);


    
    assert(r == 0);

    pager.ID = -1;
    preempter.ID = -1;
    l4_thread_ex_regs(t4, (dword_t) thread_1 ,
		      (dword_t) &thread_1_stack[127],
		      &preempter, &pager, &old_eip, &old_sp);
    
    sprintf(c,"ip=0x%lx\n", old_eip);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
    assert(r == 0);

    sprintf(c,"sp=0x%lx\n", old_sp);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
    assert(r == 0);
  }
}
