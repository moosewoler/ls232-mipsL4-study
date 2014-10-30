/****************************************************************************
 * $Id: serial.c,v 1.3 1998/01/22 05:48:20 kevine Exp $
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
#include "z85230.h"
#include "termio.h"
#include <r4kc0.h>
#include <stdarg.h>
#include <kernel/panic.h>
#include <kernel/kutils.h>
#include <kernel/trace.h>
#include <kernel/u4600.h>
#include <l4/types.h>
#include <l4/ipc.h>
#include <l4/sigma0.h>


extern int  pzscc();
int devinit();

typedef void * Addr;
typedef int iFunc();

#define MPSC_BASE       Z85230_BASE

#define PHYS_TO_K1(x) PHYS_TO_CKSEG1(x)

typedef struct ConfigEntry {
  Addr devinfo;
  int chan;
  iFunc *handler;
  int rxqsize;
  int brate;
} ConfigEntry;

ConfigEntry     ConfigTable[] =
{
  /* p4000 has swapped mpsc ports */
  {(Addr)PHYS_TO_K1(MPSC_BASE+2), 0, pzscc, 256, HOST_BAUD},
  {(Addr)PHYS_TO_K1(MPSC_BASE+0), 1, pzscc, 256, HOST_BAUD},
  {0}};

int devinit ()
{
  int             i, brate;
  ConfigEntry    *q;

  for (i = 0; ConfigTable[i].devinfo; i++) {
    q = &ConfigTable[i];
    if (q->chan == 0)
    {
      (*q->handler) (OP_INIT, q->devinfo);
    }
    brate = q->brate;
    (*q->handler) (OP_BAUD, q->devinfo, q->chan, brate);
  }
    return (0);
}


void outch(int c)
{
  LABEL("outc");
    
  ConfigEntry    *q;
  
  /*  TRACE(T_PROCS);*/
  q = &ConfigTable[0];

  /* wait to transmit */
  while(!(*q->handler) (OP_TXRDY, q->devinfo, 0));
  pzscc( OP_TX, q->devinfo, 0, c);
}

int inch(void)
{
    ConfigEntry    *q;
  q = &ConfigTable[0];

  /* wait to transmit */
  while(!(*q->handler) (OP_RXRDY, q->devinfo, 0));
  return pzscc( OP_RX, q->devinfo, 0,0);
}

