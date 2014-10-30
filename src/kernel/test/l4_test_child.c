/****************************************************************************
 * $Id: l4_test_child.c,v 1.8 1998/12/28 12:23:16 gernot Exp $
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

dword_t mbuf[100];
l4_msghdr_t *mhdr;
char cbuf1[20], cbuf2[20], cbuf3[20];
void main(void)
{
  l4_threadid_t printer, tid, tmptid;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  int r;
  l4_fpage_t fp;
  dword_t map_mask;
  l4_strdope_t *strdope;
  
  char *c;

  c = (char *) &msg.reg[0];
  
  r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  printer.ID = msg.reg[0];

  sprintf(c, "child fpage received from pager PASSED\n");
  
  r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);

  sprintf(c, "child unmapping.....\n");
  r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  fp = l4_fpage(0,64,0,0);
  map_mask = L4_FP_ALL_SPACES | L4_FP_FLUSH_PAGE;
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  l4_fpage_unmap(fp, map_mask);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  sprintf(c, "..... should have produced page faults\n");
  r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  /* test deceiving stuff */
  /* this deceit should fail */
  r = l4_mips_ipc_reply_deceiving_and_wait(tid, SIGMA0_TID,
					   L4_IPC_SHORT_MSG, &msg,
					   &tmptid, L4_IPC_SHORT_MSG, &msg,
					   L4_IPC_NEVER, &result);

  if ((r != 0) ||
      (result.md.fpage_received != 0) ||
      (result.md.msg_deceited != 1) ||
      (result.md.src_inside != 0) ||
      (tmptid.ID != SIGMA0_TID.ID))
  {
    sprintf(c, "got tid 0x%lx\n",tmptid.ID);
    r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
    sprintf(c, "outer -> inner ipc deceit test FAILED\n");
  }
  else
  {
    sprintf(c, "outer -> inner ipc deceit test PASSED\n");
  }       
  r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
   r = l4_mips_ipc_reply_and_wait(tid, 
				  L4_IPC_SHORT_MSG, &msg,
				  &tmptid, L4_IPC_SHORT_MSG, &msg,
				  L4_IPC_NEVER, &result);
   if (r != L4_IPC_REMSGCUT)
   {
     sprintf(c, "short recv sent direct msg FAILED\n");
   }
   else
   {
     sprintf(c, "short recv sent direct msg PASSED\n");
   }
   r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);

   mhdr = (l4_msghdr_t *) &mbuf[0];
   mhdr->rcv_fpage.fpage = 0;
   mhdr->size_dope.md.dwords = 60;
   mhdr->size_dope.md.strings = 3;

   sprintf(c, "recv buffer 0x%p\n", mhdr);
   r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);
   
   r = l4_mips_ipc_wait(&tmptid, (void *) mhdr, &msg,
			L4_IPC_NEVER, &result);
   if ((r != 0) ||
       (result.msgdope != msg.reg[0]) ||
       (mbuf[3+2] != 0x123454321))
   {
     sprintf(c, "r 0x%x dwords 0x%x, val 0x%lx\n",
	     r, result.md.dwords, mbuf[3+2]);
     r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
     sprintf(c, "recv direct string FAILED\n");
   }
   else
   {
     sprintf(c, "recv direct string PASSED\n");
   }
   
   r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);

   mhdr->snd_dope.md.dwords = 52;
   mhdr->snd_dope.md.strings = 0;
   mbuf[3] = 0x1234567890;
   mbuf[3+51] = 0x123454321;
   mbuf[3+52] = 0x126367482;

   strdope = (l4_strdope_t *) &mbuf[3+60];
   strdope[0].snd_size = strdope[1].snd_size = strdope[2].snd_size = 0;
   strdope[0].rcv_size = strdope[1].rcv_size = strdope[2].rcv_size = 20;
   strdope[0].rcv_str = (dword_t) cbuf1;
   strdope[1].rcv_str = (dword_t) cbuf2;
   strdope[2].rcv_str = (dword_t) cbuf3;

   r = l4_mips_ipc_reply_and_wait(tid, 
				  (void *) mhdr, &msg,
				  &tmptid, (void *) mhdr, &msg,
				  L4_IPC_NEVER, &result);

   if ((r != 0) ||
       (result.md.strings != 3) ||
       (result.md.dwords != 0) ||
       (strcmp(cbuf1,"test string XYZ") != 0) ||
       (strcmp(cbuf2,"test string XYZ") != 0) ||
       (strcmp(cbuf3,"test string XYZ") != 0))
   {
     assert(0);
     sprintf(c,"msg dope 0x%lx\n", result.msgdope);
     r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
     sprintf(c, "recv indirect string FAILED\n");
   }
   else
   {
     sprintf(c, "recv indirect string PASSED\n");
   }
   r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);
   
   mhdr->rcv_fpage.fpage = 0;
   mhdr->size_dope.md.dwords = 0;
   mhdr->size_dope.md.strings = 1;
   mhdr->snd_dope.md.dwords = 0;
   mhdr->snd_dope.md.strings = 1;
   strdope = (l4_strdope_t *) &mbuf[3];
   strdope[0].snd_size = strlen(cbuf1) + 1;
   strdope[0].rcv_size = 0;
   strdope[0].snd_str = (dword_t) cbuf1;
     
   r = l4_mips_ipc_reply_and_wait(tid, 
				  (void *) mhdr, &msg,
				  &tmptid, (void *) mhdr, &msg,
				  L4_IPC_NEVER, &result);

   if (r != L4_IPC_SEMSGCUT)
   {
     sprintf(c,"msg dope 0x%lx\n", result.msgdope);
      r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);
     sprintf(c,"send cut indirect string FAILED\n");
   }
   else
   {
     sprintf(c,"send cut indirect string PASSED\n");
   }
   r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);
   
#define TEST_ADDR (4*1024*1024)
#define TEST_ADDR2 (5*1024*1024)
   
   strdope[0].snd_size =  strlen("ipc fault") + 1;
   strdope[0].snd_str = TEST_ADDR;
   strdope[0].rcv_size = 20;
   strdope[0].rcv_str = TEST_ADDR2;

   r = l4_mips_ipc_reply_and_wait(tid, 
				  (void *) mhdr, &msg,
				  &tmptid, (void *) mhdr, &msg,
				  L4_IPC_NEVER, &result);


   if ((r != 0) ||
       (result.md.strings != 1) ||
       (result.md.dwords != 0) ||
       (strcmp((char*)TEST_ADDR2,"ipc fault") != 0))
   {
     sprintf(c,"msg dope 0x%lx\n", result.msgdope);
     r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
     sprintf(c, "recv indirect string with page fault FAILED\n");
   }
   else
   {
     sprintf(c, "recv indirect string with page fault PASSED\n");
   }
   r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);

   
   mhdr->rcv_fpage.fpage = 0;
   mhdr->size_dope.md.dwords = 0;
   mhdr->size_dope.md.strings = 1;
   mhdr->snd_dope.md.dwords = 0;
   mhdr->snd_dope.md.strings = 1;
   strdope = (l4_strdope_t *) &mbuf[3];
   strdope[0].snd_size = strlen(cbuf1) + 1;
   strdope[0].rcv_size = 0;
   strdope[0].snd_str = (dword_t) cbuf1;

   r = l4_mips_ipc_reply_and_wait(tid, 
				  (void *) mhdr, &msg,
				  &tmptid, (void *) mhdr, &msg,
				  L4_IPC_NEVER, &result);

   if ((r != L4_IPC_SERCVPFTO))
   {
     sprintf(c,"msg dope 0x%lx\n", result.msgdope);
     r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
     sprintf(c, "send indirect string with failed recv pfault FAILED\n");
   }
   else
   {
     sprintf(c, "send indirect string with failed recv pfault PASSED\n");
   }
   r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);

   mhdr->rcv_fpage.fpage = 0;
   mhdr->size_dope.md.dwords = 0;
   mhdr->size_dope.md.strings = 1;
   mhdr->snd_dope.md.dwords = 0;
   mhdr->snd_dope.md.strings = 0;
   strdope[0].snd_size = 0;
   strdope[0].rcv_size = 20;
   strdope[0].rcv_str = (dword_t) cbuf1;
   
   r = l4_mips_ipc_wait(&tmptid, (void *) mhdr, &msg,
			L4_IPC_NEVER, &result);
   
   if ((r != L4_IPC_RESNDPFTO))
   {
     sprintf(c,"msg dope 0x%lx\n", result.msgdope);
     r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
     sprintf(c, "rcv indirect string with failed sndr pfault FAILED\n");
   }
   else
   {
     sprintf(c, "rcv indirect string with failed sndr pfault PASSED\n");
   }
   r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);
   
   mhdr->rcv_fpage.fpage = 0;
   mhdr->size_dope.md.dwords = 0;
   mhdr->size_dope.md.strings = 1;
   mhdr->snd_dope.md.dwords = 0;
   mhdr->snd_dope.md.strings = 0;
   strdope[0].snd_size = 0;
   strdope[0].rcv_size = 1024*1024;
   strdope[0].rcv_str = (dword_t) TEST_ADDR2;
   
   r = l4_mips_ipc_wait(&tmptid, (void *) mhdr, &msg,
			L4_IPC_TIMEOUT(0,0,0,0,5,6), &result);
   
   if ((r != L4_IPC_REABORTED))
   {
     sprintf(c,"msg dope 0x%lx\n", result.msgdope);
     r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_NEVER, &result);
     sprintf(c, "rcv indirect string with ex_reg abort FAILED\n");
   }
   else
   {
     sprintf(c, "rcv indirect string with ex_reg abort PASSED\n");
   }
   r = l4_mips_ipc_send(printer, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_NEVER, &result);
   
   while(1);
}


