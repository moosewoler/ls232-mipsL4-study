/****************************************************************************
 * $Id: main.c,v 1.4 1999/01/01 07:52:45 gernot Exp $
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

#include <stdio.h>
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>
#include <kernel/dit.h>

/* assumed thread id of serial driver */
static const l4_threadid_t SERIAL_TID = {0x1002000000060001LL};

/****************************************************************************
 * initial servers need to allocate space for their own stack
 * and point _sp at it. This is a "magic" address which will be picked
 * up by the loader.
 ****************************************************************************/
#define STACK_SIZE 1024
dword_t _stack[STACK_SIZE];
dword_t *_sp = &_stack[STACK_SIZE-1];


void main(void)
{
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t result;
  l4_threadid_t id;
  l4_kernel_info *k;
  Dit_Dhdr *dhdr;
  Dit_Phdr *phdr;
  char *c;
  int r,i;

/****************************************************************************
 * first figure out my thread id and then ipc a hello message to serial
 * driver
 ****************************************************************************/ 

  /* point c at start of 64 byte register message buffer */
  c = (char *) &msg.reg[0]; 

  id = l4_myself();
  sprintf(c, "Hello World, my thread id is 0x%llx\n",
	  id.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);
  
/****************************************************************************
 * map the kernel info page and print out stuff inside
 ****************************************************************************/
  msg.reg[0] = SIGMA0_KERNEL_INFO_MAP;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
  assert(r == 0);

  k = (l4_kernel_info *) msg.reg[0];

  sprintf(c, "%4s version %hd build %hd\n",
	  (char *) &(k->magic), k->version, k->build);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  sprintf(c, "Memory size %dMB\n", k->memory_size / 1024 / 1024);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);
  sprintf(c, "L4 reserved below 0x%llx and above 0x%llx\n",
	  k->kernel, k->kernel_data);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);


/****************************************************************************
 * get the dit header and print out what was loaded
 ****************************************************************************/ 
  
  sprintf(c, "The boot image contains the following\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);
  dhdr = (Dit_Dhdr *) k->dit_hdr;
  for (phdr = (Dit_Phdr *) ((dword_t)dhdr +  dhdr->d_phoff), i = 0;
       i < dhdr->d_phnum;
       phdr++, i++)
  {
    sprintf(c, "%-12s [0x%-08lx,0x%-08lx] size 0x%-08lx", phdr->p_name,
	    phdr->p_base, phdr->p_base+phdr->p_size, phdr->p_size);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
    if ((dword_t)&phdr >= phdr->p_base  &&
	(dword_t)&phdr < phdr->p_base + phdr->p_size) {
      sprintf(c, " <= that't me!\n");
    } else {
      sprintf(c, "\n");
    }
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
  }


/****************************************************************************
 * initial servers can't exit, could leave spinning while giving time slice
 * away with something like
 *
 * while (1) l4_thread_switch(L4_NIL_ID);
 *
 * instead, we will loop printing out the time every 10 seconds
 ****************************************************************************/

  while (1)
  {
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,10,5,0,0),&result);
    assert(r == L4_IPC_RETIMEOUT);
    sprintf(c, "millisecs since booting = %lld\n",
	    k->clock);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
  }

}
