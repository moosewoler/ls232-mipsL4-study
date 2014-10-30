/****************************************************************************
 * $Id: ipc_test_timestamp.c,v 1.3 1998/12/28 12:23:15 gernot Exp $
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
#include <string.h>
#include <libc.h>
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>
#include <kernel/machine.h>
#include <kernel/gt64010a.h>

#ifdef U4600

#define GTXW(x) (*(volatile unsigned int *)(_gt64010 + (x)))
char * const _gt64010 = (char *)GT_BASE_ADDR;
static const l4_threadid_t SERIAL_TID = {0x1002000000060001};

unsigned int byte_swap(unsigned int x)
{
  return (((x & 0x000000FF) << 24) | 
	  ((x & 0x0000FF00) << 8)  |
	  ((x & 0x00FF0000) >> 8)  |
	  ((x & 0xFF000000) >> 24));
}

void timestamp_init(void)
{
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t result;
  l4_threadid_t tid, page, pid;
  int r;
  char *c;
  l4_kernel_info *k;
  
  c = (char *) &msg.reg[0];
  /* map the gt memory port memory  */
  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = GT_BASE_ADDR;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
  
  msg.reg[0] = SIGMA0_KERNEL_INFO_MAP;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
  k = (l4_kernel_info *) msg.reg[0];
  /* initilise a timer */
  pid = tid = l4_myself();
  pid.id.lthread = 1;
  page.ID = -1;


  /* disable timer */
  GTXW(GT_TIMER_CNTRL) = 0;

  /* set up timer */
  GTXW(GT_TIMER0) = byte_swap(-1);
  
  /* enable timer */
  GTXW(GT_TIMER_CNTRL) = byte_swap(GT_TIM0_EN | GT_TIM0_TMR);

}

unsigned int timestamp(void)
{
  return byte_swap(GTXW(GT_TIMER0));
}

#endif
