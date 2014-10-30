/****************************************************************************
 * $Id: tt1.c,v 1.3 1998/01/22 05:47:54 kevine Exp $
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

static const l4_threadid_t upperpagerid = {0x10020000000a0c01};
static const l4_threadid_t printer = {0x10020000000a1001};
void pager(void);
dword_t pager_stack[128];

void main(void)
{
  l4_threadid_t tid, t1, t2, page, pre, pagerid;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  int r;
  char *c;
  dword_t oip, osp;

  c = (char *) &msg.reg[0];
  tid = l4_myself();
  sprintf(c,"task_new test task started\r\n");
  r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
  pagerid = l4_myself();
  pagerid.id.lthread = 1;
  pre.ID = -1;
  page.ID = upperpagerid.ID;
  
  l4_thread_ex_regs(pagerid,
		    (dword_t) pager,
		    (dword_t) &pager_stack[127],
		    &pre,
		    &page,
		    &oip,
		    &osp);


  t1.id.task = tid.id.task + 1;
  t2.id.task = tid.id.task + 2;
  t1 = l4_task_new(t1, 0,  
		     0x0008fff8,
		     0x00090118,
		     pagerid, l4_myself());

  t2 = l4_task_new(t2, 0,  
		     0x00097ff8,
		     0x00098118,
		     pagerid, l4_myself());

  while(1);
}


void pager(void)
{
  int r;
  l4_ipc_reg_msg_t pmsg; 
  l4_threadid_t thrdid;
  l4_msgdope_t result;
  dword_t fault_addr;
  l4_snd_fpage_t *fp;


  while(1)
  {
    r = l4_mips_ipc_wait(&thrdid, L4_IPC_SHORT_MSG, &pmsg,
			 L4_IPC_NEVER, &result);

    fault_addr =  (pmsg.reg[0] & (~(dword_t) 3));
    
    
    /* map page */
    pmsg.reg[0] = fault_addr | 2;
    r = l4_mips_ipc_call(upperpagerid, L4_IPC_SHORT_MSG, &pmsg,
			 (void *)
			 ((dword_t) L4_IPC_SHORT_FPAGE |
			  (L4_WHOLE_ADDRESS_SPACE << 2 )),
			 &pmsg, L4_IPC_NEVER, &result);
    
    
    /* send mapping to client */
    fp = (l4_snd_fpage_t *)&pmsg.reg[0];
    fp[0].snd_base = fault_addr;
    fp[0].fpage = l4_fpage(fault_addr, 12, 1, 0);
    fp[1].fpage.fpage = 0;
    
    r = l4_mips_ipc_send(thrdid, 
			 L4_IPC_SHORT_FPAGE, &pmsg,
			 L4_IPC_NEVER, &result);
  }
}
