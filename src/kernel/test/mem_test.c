/****************************************************************************
 * $Id: mem_test.c,v 1.3 1998/12/28 12:23:17 gernot Exp $
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

#define REFS 1000000
#define MEM_BASE  (8*1024*1024)

void main()
{
  l4_msgdope_t result;
  int r;
  l4_ipc_reg_msg_t msg;
  char *c;
  l4_threadid_t tid;
  unsigned long count, time0, time, dummy;
#ifndef P4000
  l4_kernel_info *kip;
#endif
  


  count = 0;
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "Mem tester... thread id 0x%lx\n",tid.ID);

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);

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

    /**************************************************************/
    c = (char *) &msg.reg[0];
    sprintf(c, "%d mem refs over 8K region\n", REFS );
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,3,5,0,0),&result);
#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    time0 = msg.reg[0];
#else
    time0 = kip->clock;
#endif

    for (count = 0; count < REFS*8; count += 8)
    {
      dummy = *(volatile long *)(MEM_BASE + (count << 51 >> 51));
    }
#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    time = msg.reg[0]
#else
    time = kip->clock;
#endif
    sprintf(c,"time = %ld\n", time - time0);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);

    /**************************************************************/
    c = (char *) &msg.reg[0];
    sprintf(c, "%d mem refs over 8K region 32 bytes apart\n", REFS );
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,3,5,0,0),&result);
#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    time0 = msg.reg[0];
#else
    time0 = kip->clock;
#endif
    for (count = 0; count < REFS*32; count += 32)
    {
      dummy = *(volatile long *)(MEM_BASE + (count << 51 >> 51));
    }
#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    time = msg.reg[0]
#else
    time = kip->clock;
#endif
    sprintf(c,"time = %ld\n", time - time0);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
    
    /**************************************************************/
    
    c = (char *) &msg.reg[0];
    sprintf(c, "%d mem refs over 512K region\n", REFS );
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,3,5,0,0),&result);
#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    time0 = msg.reg[0];
#else
    time0 = kip->clock;
#endif
    for (count = 0; count < REFS*8; count+= 8)
    {
      dummy = *(volatile long *)(MEM_BASE + (count << 45 >> 45));
    }
#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    time = msg.reg[0]
#else
    time = kip->clock;
#endif
    sprintf(c,"time = %ld\n", time - time0);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
    
    /**************************************************************/

    c = (char *) &msg.reg[0];
    sprintf(c, "%d mem refs over 512K region 32 bytes apart\n", REFS );
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,3,5,0,0),&result);
#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    time0 = msg.reg[0];
#else
    time0 = kip->clock;
#endif
    for (count = 0; count < REFS*32; count += 32)
    {
      dummy = *(volatile long *)(MEM_BASE + (count  << 45 >> 45));
    }
#ifdef P4000
    r = l4_mips_ipc_call(SONIC_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    time = msg.reg[0]
#else
    time = kip->clock;
#endif
    sprintf(c,"time = %ld\n", time - time0);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);

    
    sprintf(c,"====================================\n");
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
  }
}
