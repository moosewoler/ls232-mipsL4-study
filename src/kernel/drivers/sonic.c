/****************************************************************************
 * $Id: sonic.c,v 1.7 1998/01/22 05:47:06 kevine Exp $
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

#include "sonic.h"
#include <kernel/p4000i.h>
#include <libc.h>
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>

static const l4_threadid_t SERIAL_TID = {0x1002000000060001};

main()
{
  l4_msgdope_t result;
  int r;
  l4_threadid_t tid;
  l4_ipc_reg_msg_t msg;
  struct sonic_reg *s;
  int *net_reset;
  unsigned int time;
  
  /* map the reset control memory */
  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = RESET_BASE;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
  
  assert(r == 0);

  /* map the sonic registers */
  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = SONIC_BASE;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);

  assert(r == 0);

  /* take sonic out of reset state */
  net_reset = (int *) NET_RESET;
  *net_reset = -1;

  s = (struct sonic_reg *) SONIC_BASE;
  
  sprintf((char *) &msg.reg[0], "SONIC Rev %d enabled\r\n", s->sr);

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		  L4_IPC_NEVER, &result);

  assert(r == 0);

  /* start the timer */
  s->wt0 = 0xffff;
  s->wt1 = 0xffff;
  s->cr = S_CR_ST;

  r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG,
		       &msg,  L4_IPC_NEVER, &result);
  assert(r == 0);
  
  while (1)
  {
    msg.reg[0] = s->wt0 | (s->wt1 << 16);
    r = l4_mips_ipc_reply_and_wait(tid, L4_IPC_SHORT_MSG, &msg,
				   &tid, L4_IPC_SHORT_MSG, &msg,
				   L4_IPC_NEVER, &result);
    assert(r == 0);
  }
}
