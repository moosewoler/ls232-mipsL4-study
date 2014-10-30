/****************************************************************************
 * $Id: i8254.c,v 1.5 1998/01/22 05:46:58 kevine Exp $
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

#include <kernel/indy.h>
#include <libc.h>
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>

#define STACK_SIZE 512
unsigned long _stack[STACK_SIZE];
unsigned long *_sp = &_stack[STACK_SIZE -1];

/*
 * Definitions for 8254 programmable interval timer
 *
 * NOTE: counter2 is clocked at MASTER_FREQ (defined below), the
 * output of counter2 is the clock for both counter0 and counter1.
 * For the IP17/IP20/IP22, counter0 output is tied to interrupt 2 for the
 * scheduling clock, and interrupt 3 for the 'fast' clock.
 * For all other CPUs, counter0 output is tied to Interrupt 2 to act 
 * as the scheduling clock and the output of counter1 is tied to 
 * Interrupt 4 to act as the profiling clock.
 */
/*
 * control word definitions
 */

#define PTCW_SC(x)      ((x)<<6)        /* select counter x */
#define PTCW_RBCMD      (3<<6)          /* read-back command */
#define PTCW_CLCMD      (0<<4)          /* counter latch command */
#define PTCW_LSB        (1<<4)          /* r/w least signif. byte only */
#define PTCW_MSB        (2<<4)          /* r/w most signif. byte only */
#define PTCW_16B        (3<<4)          /* r/w 16 bits, lsb then msb */
#define PTCW_MODE(x)    ((x)<<1)        /* set mode to x */
#define PTCW_BCD        0x1             /* operate in BCD mode */

/*
 * Mode definitions
 */
#define MODE_ITC        0               /* interrupt on terminal count */
#define MODE_HROS       1               /* hw retriggerable one-shot */
#define MODE_RG         2               /* rate generator */
#define MODE_SQW        3               /* square wave generator */
#define MODE_STS        4               /* software triggered strobe */
#define MODE_HTS        5               /* hardware triggered strobe */

static const l4_threadid_t SERIAL_TID = {0x1002000000060001};
unsigned int timestamp(void);

main()
{
  l4_msgdope_t result;
  int r;
  l4_threadid_t tid;
  l4_ipc_reg_msg_t msg;
  struct pt_clock *pt;
  unsigned int time;
  unsigned char *c;
  
  /* map the reset control memory */
  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = HPC3_INT_ADDR;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
  
  assert(r == 0);

  /* take sonic out of reset state */

  pt = (struct pt_clock *) PT_CLOCK_ADDR;
  
  sprintf((char *) &msg.reg[0], "Initialising 8254 timer\r\n");

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		  L4_IPC_NEVER, &result);

  assert(r == 0);
  
  /* start the timer 2 at max value*/
  pt->pt_control = PTCW_SC(2) | PTCW_16B |   PTCW_MODE(MODE_RG) ;
  sync_wait();
  pt->pt_counter2 = 0xff;
  sync_wait();
  pt->pt_counter2 = 0xff;
  sync_wait();
  pt->pt_control = PTCW_SC(1) | PTCW_16B |  PTCW_MODE(MODE_RG) ;
  sync_wait();
  pt->pt_counter1 = 0xff;
  sync_wait();
  pt->pt_counter1 = 0xff;
  sync_wait();
  
  r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG,
		       &msg,  L4_IPC_NEVER, &result);
  assert(r == 0);
  
  while (1)
  {
    msg.reg[0] = timestamp();
    r = l4_mips_ipc_reply_and_wait(tid, L4_IPC_SHORT_MSG, &msg,
				   &tid, L4_IPC_SHORT_MSG, &msg,
				   L4_IPC_NEVER, &result);
    assert(r == 0);
  }
}
