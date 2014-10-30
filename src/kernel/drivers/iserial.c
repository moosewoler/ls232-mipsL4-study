/****************************************************************************
 * $Id: iserial.c,v 1.5 1999/03/22 06:30:57 gernot Exp $
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

#include <stdarg.h>
#include <libc.h>
#include <assert.h>
#include <l4/types.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>

#define STACK_SIZE 512
unsigned long _stack[STACK_SIZE];
unsigned long *_sp = &_stack[STACK_SIZE -1];

unsigned char rdzero(void);
unsigned char rdreg(unsigned char);
void wrreg(unsigned char, unsigned char);


void outch(int b)
{
  while (!(rdzero() & 4))
    ;
  wrreg(8,b);
  if (b == '\n')
  {
    while (!(rdzero() & 4))
      ;
    wrreg(8,'\r');
  }
}

int inch(void)
{

  while(!(rdzero() & 1));
  return rdreg(8);
}



#define BUFFSIZE 1024l

char sbuff[BUFFSIZE];
long sin;

void main()
{
  l4_msgdope_t result;
  int r;
  l4_threadid_t tid;
  int *p;
  int i;
  l4_ipc_reg_msg_t msg;
  char *m;

  
  /* map the indy insterrupt reg */
  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = 0x1fbb0000;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
  assert(r == 0);


  /* map serial port memory */
  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = 0x1fbd9000;
  
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
  assert(r == 0);

  /* make sure interrupts are off */
  wrreg(9,0);
  wrreg(1,0);

  tid = l4_myself();

  sprintf(sbuff,"\nSERIAL DRIVER: enabled, thread id 0x%llx\n", tid);
  
  sin = 0;
  while (sbuff[sin] != 0)
  {
    outch(sbuff[sin]);      
    sin ++;
  }
  
  while (1)
  {
    r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    if (r != 0)
    {
      sprintf(sbuff,"\nSERIAL DRIVER: received msg error: %x\n",
	      r);
      for (sin = 0; sbuff[sin] !=0; sin++)
      {
	outch( sbuff[sin]);
      }
    }
    else
    {
      /* sprintf(sbuff, "0x%llx: ", tid.ID);
      for (sin = 0; sbuff[sin] !=0; sin++)
      {
	outch( sbuff[sin]);
      } */
      m = (char *) &msg.reg[0];
      
      for (i = 0; i < 64; i++)
      {
	if (m[i] == 0)
	{
	  break;
	}
	else
	{
	  outch(m[i]);
	}
      }
    }
  }
}

