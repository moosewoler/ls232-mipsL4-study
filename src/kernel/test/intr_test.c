/****************************************************************************
 * $Id: intr_test.c,v 1.3 1998/01/22 05:47:40 kevine Exp $
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
#include <kernel/machine.h>

static const l4_threadid_t SERIAL_TID = {0x1002000000060001L};

int count = 0;
dword_t print_stack[128];
void print_thread (void)
{
  int r;
  l4_threadid_t thrdid;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  char *c;

  c = (char *) &msg.reg[0];
  while (1)
  {

    sprintf(c,"intr count %d\r\n",count);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    r = l4_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0), &result);
  }
}
  
void main(void)
{
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t result;
  l4_threadid_t intr, page, pre, id;
  dword_t dum;
  char *c;
  int *clear_int, dummy;
  int r;
  pic_t *pic;
  unsigned char v1,v2;

  c = (char *) &msg.reg[0];


  /* map pci interrupt controller */
  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = PCI_INT_BASE;
  
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
  
  assert(r == 0);
  
  sprintf(c,
	  "********************  intr tester ********************\n\r");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  /* test registers and print state */
  pic = (pic_t *) PCI_INT_BASE;
  
  v1 = 0xff; v2 = 0x00;
  sprintf(c, " writing 0xff in all registers of pic\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  pic -> im_lo = v1;
  pic -> im_hi = v1;
  wbflush();
  
  sprintf(c, "im lo 0x%x, im hi 0x%x\r\n", pic->im_lo
	  ,pic->im_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  sprintf(c, "ip lo 0x%x, ip hi 0x%x\r\n", pic->ip_lo
	  ,pic->ip_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
 
  pic -> im_lo = v2;
  pic -> im_hi = v2;
  wbflush();
  sprintf(c, " writing 0x00 in all registers of pic\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  sprintf(c, "im lo 0x%x, im hi 0x%x\r\n", pic->im_lo
	  ,pic->im_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  sprintf(c, "ip lo 0x%x, ip hi 0x%x\r\n", pic->ip_lo
	  ,pic->ip_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  sprintf(c, " writing 0xff in all registers of pic\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  pic -> im_lo = v1;
  pic -> im_hi = v1;
  wbflush();
  
  sprintf(c, "im lo 0x%x, im hi 0x%x\r\n", pic->im_lo
	  ,pic->im_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  sprintf(c, "ip lo 0x%x, ip hi 0x%x\r\n", pic->ip_lo
	  ,pic->ip_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
 
  pic -> im_lo = v2;
  pic -> im_hi = v2;
  wbflush();
  sprintf(c, " writing 0x00 in all registers of pic\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  sprintf(c, "im lo 0x%x, im hi 0x%x\r\n", pic->im_lo
	  ,pic->im_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  sprintf(c, "ip lo 0x%x, ip hi 0x%x\r\n", pic->ip_lo
	  ,pic->ip_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

   sprintf(c, " writing 0xff in all registers of pic\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  pic -> im_lo = v1;
  pic -> im_hi = v1;
  wbflush();
  
  sprintf(c, "im lo 0x%x, im hi 0x%x\r\n", pic->im_lo
	  ,pic->im_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  sprintf(c, "ip lo 0x%x, ip hi 0x%x\r\n", pic->ip_lo
	  ,pic->ip_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
 
  pic -> im_lo = v2;
  pic -> im_hi = v2;
  wbflush();
  sprintf(c, " writing 0x00 in all registers of pic\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  sprintf(c, "im lo 0x%x, im hi 0x%x\r\n", pic->im_lo
	  ,pic->im_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  sprintf(c, "ip lo 0x%x, ip hi 0x%x\r\n", pic->ip_lo
	  ,pic->ip_hi);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  while (1);

}
