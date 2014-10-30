/****************************************************************************
 * $Id: intr_serial.c,v 1.14 2002/09/16 06:02:25 cls Exp $
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
#include "z85230.h"
#include "termio.h"
 
#define STACK_SIZE 512
unsigned long _stack[STACK_SIZE];
unsigned long *_sp = &_stack[STACK_SIZE -1];

typedef volatile struct zsccdev *zsccdp;

volatile zsccdp porta =  (zsccdp)(long)(Z85230_BASE + 2);
volatile zsccdp portb =  (zsccdp)(long)(Z85230_BASE + 0);

#define CPUCLK  100000000	 /* CPU clock rate 100 MHz */
#define ZSCCCLK	14745600	 /* ZSCC clock rate 14.7456 MHz */

/* experimental evidence dictates that we need to wait a few ZSCC clock
 * cycles between register reads and writes. the following works well.
 */
#define DELAY	{ int i; for (i = 0; i < (CPUCLK / ZSCCCLK * 6); i++); }

static const int zsccbrtc[] = {
    -1,
    BRTC (ZSCCCLK, 50),
    BRTC (ZSCCCLK, 75),
    BRTC (ZSCCCLK, 110),
    BRTC (ZSCCCLK, 134),
    BRTC (ZSCCCLK, 150),
    BRTC (ZSCCCLK, 200),
    BRTC (ZSCCCLK, 300),
    BRTC (ZSCCCLK, 600),
    BRTC (ZSCCCLK, 1200),
    BRTC (ZSCCCLK, 1800),
    BRTC (ZSCCCLK, 2400),
    BRTC (ZSCCCLK, 4800),
    BRTC (ZSCCCLK, 9600),
    BRTC (ZSCCCLK, 19200),
    BRTC (ZSCCCLK, 38400)};


static void
zsccputreg (volatile zsccdp dp, unsigned char reg, unsigned char val)
{
  if (reg != zWR0) {
      DELAY;
      dp->ucmd = zWR0_REG | reg; 
  }
  DELAY;
  dp->ucmd = val;
}

static unsigned char
zsccgetreg (volatile zsccdp dp, unsigned char reg)
{
  if (reg != zRR0) {
    DELAY;
    dp->ucmd = zWR0_REG | reg;
  }
  DELAY;
  return (dp->ucmd);
}

static int
zsccinit (volatile zsccdp dp)
{
    volatile zsccdp dpa, dpb;

    dpa = dp; dpb = dpa - 1;

    /* single read to get in known state */
    (void) dpa->ucmd;
    (void) dpb->ucmd;

    /* global initialisation */
    
    zsccputreg(dpa, zWR9, zWR9_HARDRESET);
    DELAY;
    DELAY;
    return 0;
}


static void
zsccflush (volatile zsccdp dp)
{
    /* wait for Tx fifo to drain */
    int timeout = 30;
    while (!(zsccgetreg (dp, zRR0) & zRR0_TXEMPTY))
	if (--timeout == 0)
	    break;
}


static int
zsccprogram (volatile zsccdp dp, int baudrate)
{
    zsccflush (dp);

    baudrate &= CBAUD;
    if (baudrate == 0)
      return 1;
    
    /*
     * See the zscc manual for details.
     */

    zsccputreg(dp, zWR4, zWR4_1STOPBIT | zWR4_CLK);
    zsccputreg(dp, zWR10, zWR10_NRZ);
    
    zsccputreg(dp, zWR14, zWR14_NOP); /* stop BRG */
    zsccputreg(dp, zWR11, zWR11_TRXCOUT| zWR11_TRXCBRG| zWR11_TCLKBRG | zWR11_RCLKBRG);
    zsccputreg(dp, zWR12,  zsccbrtc [baudrate] & 0xff);
    zsccputreg(dp, zWR13,  (zsccbrtc [baudrate] >> 8) & 0xff);
    
    zsccputreg(dp, zWR14, zWR14_BRGENABLE | zWR14_BRGSRC );
    
    zsccputreg(dp, zWR15, zWR15_WR7PEN);
    zsccputreg(dp, zWR7, zWR7P_TXEMPTYIEN);
    zsccputreg(dp, zWR1, zWR1_TXIEN | zWR1_RXIENALL);
    zsccputreg(dp, zWR9, zWR9_MIE | zWR9_INTACK);
    
    zsccputreg(dp, zWR3, zWR3_RXENABLE | zWR3_RX8BITCHAR);
    zsccputreg(dp, zWR5, zWR5_TXENABLE| zWR5_RTS | zWR5_TX8BITCHAR | zWR5_DTR);

    return 0;
}

#define FIFO_SIZE	1024
#define FIFO_SIZE_MASK	(FIFO_SIZE-1)

static unsigned char output_fifo[FIFO_SIZE];
static volatile int wnd_lo, wnd_hi;
static volatile int async = 0;

/* Queue data into FIFO */
static void fifo_queue(char *data)
{
  int next_hi = wnd_hi;
  int maxsize = sizeof(l4_ipc_reg_msg_t);
  int newline = 0;

  while ((maxsize > 0) && (*data != 0))
  {
    next_hi++;
    next_hi &= FIFO_SIZE_MASK;

    while (next_hi == wnd_lo)
    {
      /* wait for window to open */
      l4_thread_switch(L4_NIL_ID);
    }

    if ((*data == '\n') && !newline)
    {
      /* dynamically translate LF into CR-LF */
      output_fifo[wnd_hi] = '\r';
      newline = 1;
    }
    else
    {
      output_fifo[wnd_hi] = *(data++);
      newline = 0;
      maxsize--;
    }

    wnd_hi = next_hi;
  }
}

/* Transmit from FIFO */
static void fifo_transmit(void)
{
  while (1)
  {
    if (wnd_lo == wnd_hi)
    {
      /* nothing more for now */
      async = 0;
      break;
    }

    if (!(zsccgetreg(porta, zRR0) & zRR0_TXEMPTY))
    {
      /* hardware FIFO full - go asynchronous */
      async = 1;
      break;
    }

    porta->udata = output_fifo[wnd_lo];

    wnd_lo++;
    wnd_lo &= FIFO_SIZE_MASK;
  }
}

#define ISR_STACK_SIZE	128
static dword_t isr_stack[ISR_STACK_SIZE];
static l4_threadid_t rcvtid;

/* Interrupt service thread */
static void isr_thread(void)
{
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t result;
  l4_threadid_t intr;
  unsigned char intcode;

  intr.ID = 0;
  intr.id.version_low = INT_Z85230;
  l4_mips_ipc_receive(intr, L4_IPC_SHORT_MSG, &msg,
		      L4_IPC_TIMEOUT(0,0,0,1,0,0), &result);

  while (1)
  {
    /* wait for interrupt */
    l4_mips_ipc_receive(intr, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);

    intcode = zsccgetreg(portb, zRR2B);

    switch (intcode & zRR2B_STATUSMASK)
    {
      case zRR2B_TXEMPTY:
	if (async)
	{
	  fifo_transmit();
	}
	else
	{
	  zsccputreg(porta, zWR0, zWR0_RESETTXINT);
	}
	break;

      case zRR2B_RXAVAIL:
	msg.reg[0] = (dword_t)porta->udata;

	if (msg.reg[0] == 0)
	{
	  assert(!"break received");
	}
	else if (rcvtid.ID != 0)
	{
	  if (msg.reg[0] == '\r')
	  {
	    msg.reg[0] = '\n';
	  }
	  if (l4_mips_ipc_send(rcvtid, L4_IPC_SHORT_MSG, &msg,
			       L4_IPC_TIMEOUT(1,10,0,0,0,0), &result) != 0)
	  {
	    rcvtid.ID = 0;
	  }
	}
	break;

      case zRR2B_SPECIAL:
	zsccputreg(porta, zWR0, zWR0_ERRORRESET);
	break;

      case zRR2B_NONE:
	break;

      default:
	/* if we assert here, try increasing the delay loop */
	assert(!"unexpected interrupt");
    }

    /* reset interrupt under service bits */
    zsccputreg(porta, zWR0, zWR0_RESETIUS);
  }
}

/* Serial driver entrypoint */
void main(void)
{
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t result;
  l4_threadid_t tid, except;
  dword_t dummy;
  int len, r;

  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = Z85230_BASE;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		   L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		   &msg, L4_IPC_NEVER, &result);
  assert(r == 0);

  zsccinit(porta);
  zsccprogram(porta, HOST_BAUD);

  /* start interrupt service thread */
  tid = l4_myself();
  tid.id.lthread = 1;
  except = L4_INVALID_ID;
  l4_thread_ex_regs(tid, (dword_t)isr_thread,
		    (dword_t)&isr_stack[ISR_STACK_SIZE-1],
		    &except, &except, &dummy, &dummy);

  wnd_lo = 0;
  wnd_hi = sprintf(output_fifo, "Interrupt serial driver at 0x%lx\r\n", tid);

  while(1)
  {
    if (!async)
    {
      fifo_transmit();
    }

    while (l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG, &msg,
			    L4_IPC_NEVER, &result) != 0);

    if (msg.reg[0] == 0)
    {
      rcvtid.ID = msg.reg[1];  /* register receiver */
    }
    else
    {
      fifo_queue((char *)&msg);
    }
  }
}

