/****************************************************************************
 * $Id: reg_main.c,v 1.4 1999/10/23 07:10:39 gernot Exp $
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
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>
#include <kernel/dit.h>
#include <stdarg.h>

l4_ipc_reg_msg_t msg;
l4_msgdope_t result;
l4_threadid_t id;

/* Thread id of serial driver */
static const l4_threadid_t SERIAL_TID = {0x1002000000060001LL};
union kbuf
{
  dword_t align; /* ensure start of buffer is double word aligned */
  char buf[256];
} kprintbuf;

int kprintf(const char *fmt, ...)
{ /* Note: This will only print strings <=64b correctly! */
  va_list         ap;
  int             len, r;
  l4_msgdope_t result;

  va_start (ap, fmt);
  len = vsprintf (kprintbuf.buf, fmt, ap);
  va_end (ap);
  
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,
		       (l4_ipc_reg_msg_t *) kprintbuf.buf,
		       L4_IPC_NEVER, &result);
  assert(r == 0);
  return len;
}

/****************************************************************************
 * initial servers (i.e. OS) need to allocate space for their own stack
 * and point _sp at it (c.f. crtS.s)
 ****************************************************************************/
#define STACK_SIZE 1024
dword_t _stack[STACK_SIZE];
dword_t *_sp = &_stack[STACK_SIZE-1];	/* stack address used by crtS.o */

#define THREAD_STACK_SIZE 128
dword_t pager_stack[THREAD_STACK_SIZE];
dword_t excepter_stack[THREAD_STACK_SIZE];
dword_t page_faulter_stack[THREAD_STACK_SIZE];
extern void excepter(void);
extern void page_faulter(void);
dword_t save[2];

l4_kernel_info *k;

void pager(void)
{
  int r;
  l4_ipc_reg_msg_t msg;
  l4_threadid_t thrdid, myid;
  l4_msgdope_t result;
  dword_t fault, fault_addr;
  l4_snd_fpage_t *fp;

  myid = l4_myself();
  kprintf("pager:\tHello world, I'm\t0x%llx\n", myid.ID);

  while(1)
  {
    /* wait for a page fault message */
    r = l4_mips_ipc_wait(&thrdid, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    fault = msg.reg[0];		/* for later printing */

    /* mask out lower bits */
    fault_addr =  (fault & (~(dword_t) 3));

    /* don't bother to check if fault address is valid */
    
    /* attempt to map the page in this address space from my pager, ie sigma0
     */
    msg.reg[0] = fault_addr | 2;
    r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
			 (void *)
			 ((dword_t) L4_IPC_SHORT_FPAGE |
			  (L4_WHOLE_ADDRESS_SPACE << 2 )),
			 &msg, L4_IPC_NEVER, &result);

    /* assume it worked, really should check */
    
    /* send mapping to client */
    fp = (l4_snd_fpage_t *)&msg.reg[0];
    fp[0].snd_base = fault_addr;
    fp[0].fpage = l4_fpage(fault_addr, 12, 1, 0);
    fp[1].fpage.fpage = 0;
    
    kprintf("pager:\tfault 0x%llx\tfpage \t0x%llx 0x%llx\n",
	    fault, fp[0].snd_base, fp[0].fpage);
    r = l4_mips_ipc_send(thrdid, 
			 L4_IPC_SHORT_FPAGE, &msg,
			 L4_IPC_NEVER, &result);
  }
}

void main(void)
{
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t result;
  l4_threadid_t myid, subid, pagerid, ttyoutid, excpt, page;
  Dit_Dhdr *dhdr;
  Dit_Phdr *phdr;
  char *c;
  int r,i;
  dword_t oip, osp, subip, subsp;
  l4_threadid_t invalid = L4_INVALID_ID;
  dword_t dummy;
/****************************************************************************
 * first figure out my thread id and then ipc a hello message to serial
 * driver
 ****************************************************************************/ 

  /* point c at start of 64 byte register message buffer */
  c = (char *) &msg.reg[0]; 

  myid = l4_myself();
  kprintf("main:\tHello world! I'm\t0x%llx\n", myid.ID);
  
/****************************************************************************
 * map the kernel info page and print out stuff inside
 ****************************************************************************/
  msg.reg[0] = SIGMA0_KERNEL_INFO_MAP;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
  assert(r == 0);

  k = (l4_kernel_info *) msg.reg[0];

  kprintf("main:\t%4s version %hd build %hd\n",
	  (char *) &(k->magic), k->version, k->build);
  kprintf("main:\tMemory size %dMB\n", k->memory_size / 1024 / 1024);
  
  kprintf("main:\tL4 reserved below 0x%llx and above 0x%llx\n",
	  k->kernel, k->kernel_data);
  
  
  /* now loop through dit headers and find the first thing not run */
  
  dhdr = (Dit_Dhdr *) (k->dit_hdr);
  kprintf("main:\tKIP @ 0x%llx, DitHdr @ 0x%llx\n", k, dhdr);
  
  for (phdr = (Dit_Phdr *) ((dword_t) dhdr +  dhdr->d_phoff), i = 0;
       i < dhdr->d_phnum;
       phdr++, i++)
  {
    kprintf("main:\t%-12s addr 0x%-8lx size 0x%-8lx\n",
	    phdr->p_name, phdr->p_base, phdr->p_size);
    if (phdr->p_flags == 0)
    {
      break;
    }
  }
  

  /* now startup a pager in this address space */
  kprintf("main:\tstarting pager\n");
  pagerid = myid;
  pagerid.id.lthread = 1;   /* current thread is 0 */
  excpt.ID = -1LL;
  page.ID = -1LL;
  
  l4_thread_ex_regs(pagerid,
		    (dword_t) pager,
		    (dword_t) &pager_stack[THREAD_STACK_SIZE-1],
		    &excpt,
		    &page,
		    &oip,
		    &osp);
  kprintf("main:\tTest pager is\t0x%llx\n", pagerid.ID);


  kprintf("main:\tstarting page fault tester\n");
  subid = myid;
  subid.id.lthread = 3;
  excpt.ID = -1LL;
  page = pagerid;
  
  l4_thread_ex_regs(subid,
		    (dword_t) page_faulter,
		    (dword_t) &page_faulter_stack[THREAD_STACK_SIZE],
		    &excpt,
		    &page,
		    &oip,
		    &osp);

  /* Sleep for one second */
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  assert(r == L4_IPC_RETIMEOUT);


  kprintf("main:\tstarting page fault tester\n");
  subid = myid;
  subid.id.lthread = 4;
  excpt = myid;
  page.ID = -1LL;
  
  l4_thread_ex_regs(subid,
		    (dword_t) excepter,
		    (dword_t) &excepter_stack[THREAD_STACK_SIZE],
		    &excpt,
		    &page,
		    &oip,
		    &osp);

  /* Wait for exception */
  r = l4_mips_ipc_wait(&id, L4_IPC_SHORT_MSG, &msg, L4_IPC_NEVER, &result);
  kprintf("main: exception from 0x%llx cause=0x%x epc=0x%x\n", 
	  id.ID, msg.reg[0], msg.reg[1]); 

  kprintf("main: restarting thread at pc = 0x%x\n", msg.reg[1]+4);
  l4_thread_ex_regs(id,  msg.reg[1]+4, 0xffffffffffffffff, 
		    &invalid, &invalid, &dummy, &dummy);

  /* Sleep for one more second */
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  assert(r == L4_IPC_RETIMEOUT);

  kprintf("main: end of testing\n");
  /* Sleep for final second */
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  assert(0);
}

