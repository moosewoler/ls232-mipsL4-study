/****************************************************************************
 * $Id: l4_test.c,v 1.14 1999/02/10 04:55:46 gernot Exp $
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
#include <string.h>
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>

#include "chief_ipc.h"

#ifdef __GNUC__
static const l4_threadid_t SERIAL_TID = {0x1002000000060001LL};
static const l4_threadid_t SONIC_TID =  {0x1002000000080001LL};
static const l4_threadid_t SERVER_TID = {0x10020000000a0001LL};
static const l4_threadid_t NEW_TASK_TID =    {0x0200000LL};
#else
static const l4_threadid_t SERIAL_TID = {0x1002000000060001};
static const l4_threadid_t SONIC_TID =  {0x1002000000080001};
static const l4_threadid_t SERVER_TID = {0x10020000000a0001};
static const l4_threadid_t NEW_TASK_TID =    {0x0200000};
#endif

#define STACK_SIZE 512
unsigned long _stack[STACK_SIZE];
unsigned long *_sp = &_stack[STACK_SIZE -1];

char cbuf[128];
l4_ipc_reg_msg_t tmsg, pmsg;
l4_msgdope_t tresult;
l4_threadid_t spid, id, ntid, ttid, printerid;
dword_t thread_stack[128];
dword_t spinner_stack[128];
dword_t pager_stack[128];
dword_t printer_stack[128];
l4_threadid_t tid, page, pagerid, printerid,  pre, tmptid;
dword_t oip, osp;

dword_t mbuf[100];

#define TEST_ADDR (4*1024*1024)
#define TEST_ADDR2 (5*1024*1024)
#define LONG_LEN   32
void printer(void)
{
  int r;
  l4_threadid_t thrdid, destid, old_id, old_dest;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg, pmsg;

  old_id.ID = old_dest.ID = ~0l;
  sprintf((char*)&pmsg,"printer thread id is 0x%lx\n",l4_myself().ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &pmsg,
		       L4_IPC_NEVER, &result);

  r = l4_mips_ipc_chief_wait(&thrdid, &destid, L4_IPC_SHORT_MSG, &msg,
			     L4_IPC_NEVER, &result);
  while (1)
  {
    if (!thread_equal(thrdid, old_id) || !thread_equal(destid, old_dest)) {
      sprintf((char*)&pmsg, "printer: 0x%lx -> 0x%lx\n", thrdid.ID, destid.ID);
      r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &pmsg,
			   L4_IPC_NEVER, &result);
      old_id   = thrdid;
      old_dest = destid;
    }
    r = l4_mips_ipc_chief_reply_and_wait (SERIAL_TID, thrdid, L4_IPC_SHORT_MSG,
					  &msg, &thrdid, &destid,
					  L4_IPC_SHORT_MSG, &msg,
					  L4_IPC_NEVER, &result);
  }
}
					     

  
void pager(void)
{
  int r, first, fail;
  char *c;
  l4_threadid_t thrdid;
  l4_msgdope_t result;
  dword_t fault_addr;
  l4_snd_fpage_t *fp;

  first = 1;
  c = (char *) &pmsg.reg[0];
  sprintf(c,"pager thread id is 0x%lx\n",l4_myself().ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &pmsg,
		       L4_IPC_NEVER, &result);

  while(1)
  {
    fail  = 0;
    r = l4_mips_ipc_wait(&thrdid, L4_IPC_SHORT_MSG, &pmsg,
			 L4_IPC_NEVER, &result);

    fault_addr =  (pmsg.reg[0] & (~(dword_t) 3));
    
    if ((r != 0))
    {
      sprintf(c,"msgdope 0x%lx\n",result.msgdope);
      r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &pmsg,
			   L4_IPC_NEVER, &result);
      sprintf(c,"page fault FAILED\n");
    }
    else
    {
      sprintf(c,"page fault 0x%lx 0x%lx 0x%lx\n",
	      fault_addr, pmsg.reg[1], thrdid.ID);
    }
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &pmsg,
			 L4_IPC_NEVER, &result);
    
    /* map page */
    pmsg.reg[0] = fault_addr | 2;
    r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &pmsg,
			 (void *)
			 ((dword_t) L4_IPC_SHORT_FPAGE |
			  (L4_WHOLE_ADDRESS_SPACE << 2 )),
			 &pmsg, L4_IPC_NEVER, &result);
    
    
    if ((r != 0) ||
	(result.md.fpage_received != 1))
    {
      sprintf(c,"map message fpage receive FAILED\n");
      fail++;
    }
    else
    {
      sprintf(c,"map message fpage receive PASSED\n");
    }

    if (first)
    {
      r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &pmsg,
			   L4_IPC_NEVER, &result);
    }
    /* send mapping to client */
    fp = (l4_snd_fpage_t *)&pmsg.reg[0];
    fp[0].snd_base = fault_addr;
    fp[0].fpage = l4_fpage(fault_addr, 12, 1, 0);
    fp[1].fpage.fpage = 0;
    
    r = l4_mips_ipc_send(thrdid, 
			 L4_IPC_SHORT_FPAGE, &pmsg,
			 L4_IPC_NEVER, &result);
    if (r != 0)
    {
      sprintf(c, "pager send fpage to 0x%lx FAILED\n", thrdid.ID);
    }
    else
    {
      sprintf(c, "pager send fpage to 0x%lx PASSED\n", thrdid.ID);
    }
    if (first)
    {
      r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &pmsg,
			   L4_IPC_NEVER, &result);
    }
    first = 0;
  }
}

void spinner(void)
{
  while(1);
}

void thread(void)
{
  int r;
  char *c;
  c = (char *) &tmsg.reg[0];
  while(1)
  {
    r = l4_mips_ipc_wait(&ttid, L4_IPC_SHORT_MSG, &tmsg,
			    L4_IPC_NEVER, &tresult);
    switch(tmsg.reg[0])
    {
    case 0:
      /* simply bounce it back adding one to reg 7 */
      tmsg.reg[7]++;
      r = l4_mips_ipc_send(ttid,  L4_IPC_SHORT_MSG, &tmsg,
			   L4_IPC_NEVER, &tresult);
      break;
    case 1:
      /* bounce back results */
      tmsg.reg[0] = ntid.ID;
      tmsg.reg[1] = tresult.msgdope;
      r = l4_mips_ipc_send(id,  L4_IPC_SHORT_MSG, &tmsg,
			   L4_IPC_NEVER, &tresult);
      break;
    case 2:
      r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,100,9,0,0),&tresult);
      spid = id;
      spid.id.lthread = 1;
      pre.ID = -1;
      page.ID = -1;
  
      l4_thread_ex_regs(spid,
			(dword_t) spinner,
			(dword_t) &spinner_stack[127],
			&pre,
			&page,
			&oip,
			&osp);
      break;
    default:
      sprintf(c, "received unknown packet!!!!\n");
      r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &tmsg,
			   L4_IPC_NEVER, &tresult);
      break;
    }
  }
}

void sender_thread(void)
{
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t result;
  l4_msghdr_t *mhdr;
  l4_strdope_t *strdope;
  int r;
  char * c;

  c = (char *) &msg.reg[0];
  
  mhdr = (l4_msghdr_t *) &mbuf[0];
  mhdr->rcv_fpage.fpage = 0;
  mhdr->size_dope.md.dwords = 0;
  mhdr->size_dope.md.strings = 1;
  mhdr->snd_dope.md.dwords = 0;
  mhdr->snd_dope.md.strings = 1;

  strdope = (l4_strdope_t *) &mbuf[3];
  strdope[0].rcv_size = 0;
  strdope[0].rcv_str = 0;
  strdope[0].snd_size = LONG_LEN*1024;
  strdope[0].snd_str = (dword_t) TEST_ADDR;

  r = l4_mips_ipc_send(ntid, (void *) mhdr , &msg,
		       L4_IPC_NEVER, &result);



  if ((r != L4_IPC_SEABORTED))
   {
     sprintf(c,"msg dope 0x%lx\n", result.msgdope);
     r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
     sprintf(c, "snd indirect string with ex_reg abort FAILED\n");
   }
   else
   {
     sprintf(c, "snd indirect string with ex_reg abort PASSED\n");
   }
   r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);
   



  
  while (1);
}

            /*     #
	     ##   ##    ##       #    #    #
	     # # # #   #  #      #    ##   #
	     #  #  #  #    #     #    # #  #
	     #     #  ######     #    #  # #
	     #     #  #    #     #    #   ##
	     #     #  #    #     #    #    */



void main(void)
{
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t result;
  l4_msghdr_t *mhdr;
  l4_strdope_t *strdope;
  
  int r;
  long a;
  char *c;

  c = (char *) &msg.reg[0];
  id = l4_myself();
  
  /****************************************************************************
   * print up intro message
   ****************************************************************************
   */
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  sprintf(c,
	  "********************  L4 tester ********************\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  sprintf(c,"my thread id is 0x%lx\n",id.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);


  /****************************************************************************
   * test short ipc messages
   ****************************************************************************
   */
  
  /* create a thread to receive messages */
  tid = id;
  tid.id.lthread = 2;
  pre.ID = -1;
  page.ID = -1;
  
  l4_thread_ex_regs(tid,
		    (dword_t) -1,
		    (dword_t) -1,
		    &pre,
		    &page,
		    &oip,
		    &osp);
  if ((page.ID != (SIGMA0_TID).ID) || (oip != -1l) ||
      (osp != -1l))
  {
    sprintf(c, "pre %lx page %lx\n",pre.ID,page.ID);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
    sprintf(c, "ip %lx sp %lx\n",oip,osp);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
    sprintf(c, "l4_thread_ex_regs (probe nil thread) FAILED\n");
  }
  else
  {
    sprintf(c, "l4_thread_ex_regs (probe nil thread) PASSED\n");
  }
  
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
    
  
  l4_thread_ex_regs(tid,
		    (dword_t) thread,
		    (dword_t) &thread_stack[127],
		    &pre,
		    &page,
		    &oip,
		    &osp);
  pre.ID = -1;
  page.ID = -1;
  l4_thread_ex_regs(tid,
		    (dword_t) -1,
		    (dword_t) -1,
		    &pre,
		    &page,
		    &oip,
		    &osp);
  
  if ((page.ID != (SIGMA0_TID).ID) || (oip == -1l) ||
      (osp == -1l))
  {
    sprintf(c, "l4_thread_ex_regs (start thread) FAILED\n");
  }
  else
  {
    sprintf(c, "l4_thread_ex_regs (start thread) PASSED\n");
  }
  
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  
  
  /* call */
  msg.reg[0] = 0;
  msg.reg[7] = 0x12345678;
  
  r = l4_mips_ipc_call(tid, L4_IPC_SHORT_MSG, &msg, L4_IPC_SHORT_MSG,
		       &msg, L4_IPC_NEVER, &result);
  if ((r != 0) || (result.msgdope != 0) ||
      (msg.reg[7] != 0x12345679))
  {
    sprintf(c, "msgdope 0x%lx\n", result.msgdope);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
    sprintf(c, "l4_ipc_call FAILED\n");
  }
  else
  {
    sprintf(c, "l4_ipc_call PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  /* send_receive */
  msg.reg[0] = 0;
  msg.reg[7] = 0x123456789a;
  r = l4_mips_ipc_reply_and_wait(tid, L4_IPC_SHORT_MSG, &msg,
				  &tmptid, L4_IPC_SHORT_MSG,&msg,
				  L4_IPC_NEVER, &result);
  
  if ((r != 0) || (result.msgdope != 0) ||
      (msg.reg[7] != 0x123456789b))
  {
    sprintf(c, "l4_ipc_reply_and_wait FAILED\n");
  }
  else
  {
    sprintf(c, "l4_ipc_reply_and_wait PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  /* send_only */
  msg.reg[0] = 0;
  msg.reg[7] = 0x12345678901234;
  r = l4_mips_ipc_send(tid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  if ((r != 0) || (result.msgdope != 0))
  {
    sprintf(c, "l4_ipc_send FAILED\n");
  }
  else
  {
    sprintf(c, "l4_ipc_send PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  
  /* wait */
  r = l4_mips_ipc_wait(&tmptid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  if ((r != 0) || (result.msgdope != 0) || (msg.reg[7] != 0x12345678901235))
  {
    sprintf(c, "l4_ipc_wait FAILED\n");
  }
  else
  {
    sprintf(c, "l4_ipc_wait PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  /* recieve */
  msg.reg[0] = 0;
  msg.reg[7] = 0x123452538901234;
  r = l4_mips_ipc_send(tid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  r = l4_mips_ipc_receive(tid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  if ((r != 0) || (result.msgdope != 0) || (msg.reg[7] != 0x123452538901235))
  {
    sprintf(c, "l4_ipc_receive FAILED\n");
  }
  else
  {
    sprintf(c, "l4_ipc_receive PASSED\n");
  }

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  ntid = l4_task_new(NEW_TASK_TID, 0,  
		     0x0007fff8, 0x00080118, id,l4_myself());
  r = l4_mips_ipc_wait(&tmptid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_TIMEOUT(0,0,1,5,0,0), &result);
  if ((r != 0) || (result.msgdope != 0) ||
      (msg.reg[0] != 0x00080118) || (msg.reg[1] != 0x00080118) ||
      (ntid.ID != tmptid.ID))
  {
     sprintf(c, "l4_task_new (simple) FAILED\n");
  }
  else
  {
    sprintf(c, "l4_task_new (simple) PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  /* send_receive deceiving */
  msg.reg[0] = 1;
  r = l4_mips_ipc_reply_deceiving_and_wait(tid, ntid,
					   L4_IPC_SHORT_MSG, &msg,
					   &tmptid, L4_IPC_SHORT_MSG, &msg,
					   L4_IPC_NEVER, &result);
  if ((r != 0) || (result.md.msg_deceited != 0) ||
      (result.md.fpage_received != 0) ||
      (result.md.msg_redirected != 0) ||
      (result.md.src_inside != 0) ||
      (msg.reg[0] != ntid.ID) || (msg.reg[1] != 1))
  {
    sprintf(c, "msgdope %lx\n",result.msgdope);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
    sprintf(c, "l4_ipc_reply_deceiving_and_wait FAILED\n");
  }
  else
  {
    sprintf(c, "l4_ipc_reply_deceiving_and_wait PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  /* send only deceiving */
  msg.reg[0] = 1;
  r = l4_mips_ipc_send_deceiving(tid, ntid,
				 L4_IPC_SHORT_MSG, &msg,
				 L4_IPC_NEVER, &result);
  r = l4_mips_ipc_wait(&tmptid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  if ((r != 0) || (result.md.msg_deceited != 0) ||
      (result.md.fpage_received != 0) ||
      (result.md.msg_redirected != 0) ||
      (result.md.src_inside != 0) ||
      (msg.reg[0] != ntid.ID) || (msg.reg[1] != 1))
  {
    sprintf(c, "l4_ipc_send_deceiving FAILED\n");
  }
  else
  {
    sprintf(c, "l4_ipc_send_deceiving PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  /* non-existent dest*/
  tmptid.ID = 0;
  r = l4_mips_ipc_send(tmptid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  if ((r == 0) || ( L4_IPC_ERROR(result) != L4_IPC_ENOT_EXISTENT))
  {
    sprintf(c, "send to non-existent thread FAILED\n");
  }
  else
  {
    sprintf(c, "send to non-existent thread PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  /* check timeouts for recv and send */

  spid = id;
  spid.id.lthread = 1;
  pre.ID = -1;
  page.ID = -1;
  
  l4_thread_ex_regs(spid,
		    (dword_t) spinner,
		    (dword_t) &spinner_stack[127],
		    &pre,
		    &page,
		    &oip,
		    &osp);
  
  r = l4_mips_ipc_send(spid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_TIMEOUT(0,1,0,0,0,0),
		       &result);
  
  if ((r == 0) || ( L4_IPC_ERROR(result) != L4_IPC_SETIMEOUT))
  {
    sprintf(c, "send timeout (0) FAILED\n");
  }
  else
  {
    sprintf(c, "send timeout (0) PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  
  r = l4_mips_ipc_wait(&tmptid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_TIMEOUT(0,0,0,1,0,0),
		       &result);
  
  if ((r == 0) || ( L4_IPC_ERROR(result) != L4_IPC_RETIMEOUT))
  {
    sprintf(c, "wait timeout (0) FAILED\n");
  }
  else
  {
    sprintf(c, "wait timeout (0) PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  r = l4_mips_ipc_send(spid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_TIMEOUT(5,10,0,0,0,0),
		       &result);
  
  if ((r == 0) || ( L4_IPC_ERROR(result) != L4_IPC_SETIMEOUT))
  {
    sprintf(c, "send timeout (5ms) FAILED\n");
  }
  else
  {
    sprintf(c, "send timeout (5ms) PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  
  r = l4_mips_ipc_wait(&tmptid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_TIMEOUT(0,0,5,10,0,0),
		       &result);
  
  if ((r == 0) || ( L4_IPC_ERROR(result) != L4_IPC_RETIMEOUT))
  {
    sprintf(c, "wait timeout (5ms) FAILED\n");
  }
  else
  {
    sprintf(c, "wait timeout (5ms) PASSED\n");
  }
  
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  /* test cancellations by ex_reg */
  msg.reg[0] = 2;
  r = l4_mips_ipc_send(tid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  r = l4_mips_ipc_send(spid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  if ((r == 0) || ( L4_IPC_ERROR(result) != L4_IPC_SECANCELED))
  {
    sprintf(c, "send cancel FAILED\n");
  }
  else
  {
    sprintf(c, "send cancel PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  /****************************************************************************
   * set up pager and printer threads and restart child 
   ****************************************************************************
   */
  pagerid = id;
  pagerid.id.lthread = 3;
  pre.ID = -1;
  page.ID = -1;
  
  l4_thread_ex_regs(pagerid,
		    (dword_t) pager,
		    (dword_t) &pager_stack[127],
		    &pre,
		    &page,
		    &oip,
		    &osp);

  printerid = id;
  printerid.id.lthread = 4;
  pre.ID = -1;
  page.ID = -1;

  l4_thread_ex_regs(printerid,
		    (dword_t) printer,
		    (dword_t) &printer_stack[127],
		    &pre,
		    &page,
		    &oip,
		    &osp);
  
  ntid = l4_task_new(ntid, 0,  
		     0x0007fff8,
#ifdef __GNUC__
		     0x00080010,
#else		     
		     0x00080118,
#endif		     
		     pagerid, l4_myself());
  if (ntid.ID == 0)
  {
      sprintf(c, "task_new (replace) FAILED\n");
  }
  else
  {
    sprintf(c, "task_new (replace) PASSED\n");
  }

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  msg.reg[0] = printerid.ID;
  r = l4_mips_ipc_call(ntid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  if (( r != 0) ||
      (result.msgdope != 0))
  {
    sprintf(c, "invalid deceit test FAILED\n");
  }
  else
  {
    sprintf(c, "invalid deceit test PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);

  r = l4_mips_ipc_reply_deceiving_and_wait(ntid, SIGMA0_TID,
					   L4_IPC_SHORT_MSG, &msg,
					   &tmptid, L4_IPC_SHORT_MSG, &msg,
					   L4_IPC_NEVER, &result);
  
  /* fpage */
  /* send only deceiving */
  /* send_receive deceiving */

  /****************************************************************************
   * direct string tests
   ****************************************************************************
   */
  /* send direct string to child */

  mhdr = (l4_msghdr_t *) &mbuf[0];
  mhdr->rcv_fpage.fpage = 0;
  mhdr->size_dope.md.dwords = 97;
  mhdr->size_dope.md.strings = 0;
  mhdr->snd_dope.md.dwords = 3;
  mhdr->snd_dope.md.strings = 0;
  mbuf[3+2] = 0x123454321;
  mbuf[3+52] = 0;

  
  r = l4_mips_ipc_reply_and_wait(ntid, 
				 (void *) mhdr, &msg,
				 &tmptid, L4_IPC_SHORT_MSG, &msg,
				 L4_IPC_NEVER, &result);

  if ((r != L4_IPC_SEMSGCUT) )
  {
    sprintf(c,"result.msgdope %lx\n",result.msgdope);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    sprintf(c, "send direct, receiver unwilling FAILED\n");
  }
  else
  {
    sprintf(c, "send direct, receiver unwilling PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  msg.reg[0] = mhdr->snd_dope.msgdope;
  
  r = l4_mips_ipc_reply_and_wait(ntid, 
				 (void *) mhdr, &msg,
				 &tmptid, (void *) mhdr, &msg,
				 L4_IPC_NEVER, &result);

  if ((r != 0) ||
      (result.md.dwords != 52) ||
      (result.md.strings != 0) ||
      (mbuf[3] != 0x1234567890) || 
      (mbuf[3+52] != 0) ||
      (mbuf[3+51] != 0x123454321))
  {
    sprintf(c,"result.msgdope %lx\n",result.msgdope);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    sprintf(c, "direct msg boundary check FAILED\n");
  }
  else
  {
    sprintf(c, "direct msg boundary check PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  /* receive fpage with desciptor in memory */
  
  mhdr->rcv_fpage = l4_fpage(0,64,0,0);
  msg.reg[0] = TEST_ADDR | 2;
  
  r = l4_mips_ipc_reply_and_wait(SIGMA0_TID, 
				 L4_IPC_SHORT_MSG, &msg,
				 &tmptid, (void *) mhdr, &msg,
				 L4_IPC_NEVER, &result);

  if ((r != 0) ||
      (result.md.fpage_received != 1))
  {
    sprintf(c, "fpage recv with memory fpage option FAILED\n");
  }
  else
  {
     sprintf(c, "fpage recv with memory fpage option PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  *(volatile dword_t *) TEST_ADDR = 42;

  if (*(volatile dword_t *) TEST_ADDR != 42)
  {
     sprintf(c,"confirmation of fpage recv FAILED\n");
  }
  else
  {
     sprintf(c,"confirmation of fpage recv PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  /* test indirect string */
  strcpy(cbuf,"test string XYZ");
  mhdr->rcv_fpage.fpage = 0;
  mhdr->size_dope.md.dwords = 0;
  mhdr->size_dope.md.strings = 3;
  mhdr->snd_dope.md.dwords = 0;
  mhdr->snd_dope.md.strings = 3;
  strdope = (l4_strdope_t *) &mbuf[3];
  strdope[0].snd_size = strdope[1].snd_size = strdope[2].snd_size =
    strlen(cbuf) + 1;
  strdope[0].snd_str =  strdope[1].snd_str  = strdope[2].snd_str =
    (dword_t) cbuf;
  strdope[0].rcv_size = strdope[1].rcv_size = strdope[2].rcv_size = 0;
  
  r = l4_mips_ipc_reply_and_wait(ntid, 
				 (void *) mhdr, &msg,
				 &tmptid, (void *) mhdr, &msg,
				   L4_IPC_NEVER, &result);

  if (r != L4_IPC_REMSGCUT)
  {
    sprintf(c,"msg dope 0x%lx\n", result.msgdope);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    sprintf(c,"recv cut indirect string FAILED\n");
  }
  else
  {
    sprintf(c,"recv cut indirect string PASSED\n");
  }

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  sprintf(c,"Testing page faults during IPC - ignore SIGMA0 errors...\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  /* page fault on sender side */
  strcpy((char *) TEST_ADDR, "ipc fault");
  
  mhdr->rcv_fpage.fpage = 0;
  mhdr->size_dope.md.dwords = 0;
  mhdr->size_dope.md.strings = 1;
  mhdr->snd_dope.md.dwords = 0;
  mhdr->snd_dope.md.strings = 0;

  strdope[0].snd_size = 0;
  strdope[0].rcv_size = strlen("ipc fault") + 1;
  strdope[0].rcv_str = (dword_t) cbuf;
  r = l4_mips_ipc_wait(&tmptid, (void *) mhdr, &msg,
		       L4_IPC_NEVER, &result);  

  if ((r != 0) ||
      (result.md.strings != 1) ||
      (strcmp("ipc fault",cbuf) != 0))
  {
    sprintf(c,"msg dope 0x%lx\n", result.msgdope);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    sprintf(c,"indirect string with sender page fault FAILED\n");
  }
  else
  {
    sprintf(c,"indirect string with sender page fault PASSED\n");
  }
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  mhdr->rcv_fpage.fpage = 0;
  mhdr->size_dope.md.dwords = 0;
  mhdr->size_dope.md.strings = 1;
  mhdr->snd_dope.md.dwords = 0;
  mhdr->snd_dope.md.strings = 1;
  
  strdope[0].rcv_size = 20;
  strdope[0].rcv_str = 0;
  strdope[0].snd_size = strlen("ipc fault") + 1;
  strdope[0].snd_str = (dword_t) cbuf;

  r = l4_mips_ipc_reply_and_wait(ntid, 
				 (void *) mhdr, &msg,
				 &tmptid, (void *) mhdr, &msg,
				 L4_IPC_NEVER, &result);

  if ((r != L4_IPC_RERCVPFTO))
   {
     sprintf(c,"msg dope 0x%lx\n", result.msgdope);
     r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
     sprintf(c, "recv indirect string with failed recv pfault FAILED\n");
   }
   else
   {
     sprintf(c, "recv indirect string with failed recv pfault PASSED\n");
   }
   r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);


   strdope[0].rcv_size = 0;
   strdope[0].snd_size = strlen("ipc fault") + 1;
   strdope[0].snd_str = 0;

   r = l4_mips_ipc_reply_and_wait(ntid, 
				  (void *) mhdr, &msg,
				  &tmptid, (void *) mhdr, &msg,
				  L4_IPC_NEVER, &result);

   if ((r != L4_IPC_SESNDPFTO))
   {
     sprintf(c,"msg dope 0x%lx\n", result.msgdope);
     r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
     sprintf(c, "snd indirect string with failed sndr pfault FAILED\n");
   }
   else
   {
     sprintf(c, "snd indirect string with failed sndr pfault PASSED\n");
   }
   r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);
   
   /* set up to send large string to test ex_reging long ipc */
   for (a = TEST_ADDR; a < TEST_ADDR+ LONG_LEN*1024; a++)
   {
     *(unsigned char *) a = 123;
   }
   *(unsigned char *) (TEST_ADDR2 + (LONG_LEN-1)*1024) = 0;

   /* start a sender thread */
   spid = id;
   spid.id.lthread = 1;
   pre.ID = -1;
   page.ID = -1;
   
   l4_thread_ex_regs(spid,
		    (dword_t) sender_thread,
		    (dword_t) &spinner_stack[127],
		    &pre,
		    &page,
		    &oip,
		    &osp);
  

   
   /* wait until he sends and nail him */
   while (*( volatile unsigned char *) (TEST_ADDR2 +(LONG_LEN-1) * 1024)== 0);
   l4_thread_ex_regs(spid,
		    (dword_t) -1,
		    (dword_t) -1,
		    &pre,
		    &page,
		    &oip,
		     &osp);
   r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
   pre.ID = -1;
   page.ID = -1;
   tmptid.id.task = ntid.id.task + 1;
   tmptid = l4_task_new(tmptid, 0,  
		     0x00087ff8,
#ifdef __GNUC__
		     0x00090000,
#else		     
		     0x00088118,
#endif		     
		     pagerid, l4_myself());
    
   r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,3,5,0,0),&result);
   /* nail the hierarchy we just created */
   page.ID = 0;
   pre = l4_task_new(tmptid, id.ID,  
			 0x00087ff8,
#ifdef __GNUC__
			 0x00090000,
#else		     
			 0x00088118,
#endif		     
			 page,l4_myself());
    
   if ((pre.id.chief != id.id.task) ||
       (pre.id.task != tmptid.id.task))
   {
     sprintf(c, "ret: 0x%lx orig: 0x%lx\n", pre.ID, tmptid.ID);
     r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);

     sprintf(c, "task_new (inactive) on chief of task hierarchy FAILED\n");
   }
   else
   {
     sprintf(c, "task_new (inactive) on chief of task hierarchy PASSED\n");
   }
   r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);


    
  /* combination of all the above */
  /* call */
  /* send_receive */
  /* send_only */
  /* receive_only */
  /* send only deceiving */
  /* send_receive deceiving */

  /* check error codes */
  /* invalid send desc */
  /* invalid recv desc */
  /* send fpage, with reg_only receive */
  /* send dwords, with reg_only receive */
  /* send dwords with receive buffer too small */
  /* send strings with receive reg_only */
  /* send_strings with receive buffer to small */
  /* send 2 strings with receive only 1 */


  chief_IPC_test(50, pagerid);

  sprintf(c,
	  "****************  IPC tests complete ****************\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  while(1);
  
}
