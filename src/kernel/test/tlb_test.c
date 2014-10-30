/****************************************************************************
 * $Id: tlb_test.c,v 1.3 1998/12/28 12:23:20 gernot Exp $
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
#include <l4/sigma0.h>

#define STACK_SIZE 1024
dword_t _stack[STACK_SIZE];
dword_t *_sp = &_stack[STACK_SIZE-1];

static const l4_threadid_t SERIAL_TID = {0x1002000000060001L};

#define PROBE_REGION_SIZE (4096*2*96l)
#define PROBE_REGION_START (10*1024*1024l)

void main()
{
  int r;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  char *c;
  l4_threadid_t tid;
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "TLB test thread id is 0x%lx\n",tid.ID);

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  
  while (1)
  {
    long p;
    
    c = (char *) &msg.reg[0];
    sprintf(c, "Probing 0x%lx region at 0x%lx\n",
	    PROBE_REGION_SIZE, PROBE_REGION_START);

    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
    for (p =  PROBE_REGION_START;
	 p < PROBE_REGION_START + PROBE_REGION_SIZE;
	 p += 4096)
    {
      *((long *)p) = 0;
    }
  }
}

