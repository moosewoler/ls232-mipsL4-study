/****************************************************************************
 * $Id: map_main.c,v 1.3 1999/09/12 15:14:00 gernot Exp $
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
#include <string.h>

/* #define MIN_MAP	/* for a quick test with minimum mappings */


#define CHILD_AREA_LOG_SIZE 16
#define CHILD_AREA_SIZE (1l<<CHILD_AREA_LOG_SIZE)
#if 0
#define VA_SIZE 33		/* just a bit more than 32... */
#else
#define VA_SIZE 40		/* virtual address size for R4x00 */
#endif
#define SPARSITY (8*1024)	/* to ensure we've got enough kmem */

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
#define STACK_SIZE 128
dword_t _stack[STACK_SIZE];
dword_t *_sp = &_stack[STACK_SIZE];	/* stack address used by crtS.o */
dword_t printer_stack[STACK_SIZE];


void printer(void) {
  int              r;
  l4_msgdope_t     result;
  l4_threadid_t    thrdid;
  l4_ipc_reg_msg_t msg;

  strcpy((char*)&msg, "main:\tPrinter ready\n");
  while (1) {
    r = l4_mips_ipc_reply_and_wait(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
				   &thrdid, L4_IPC_SHORT_MSG, &msg,
				   L4_IPC_NEVER, &result);
    assert (!r);
  }
}



void main(void)
{
  volatile l4_kernel_info *k;
  l4_ipc_reg_msg_t msg, msg1;
  l4_msgdope_t result;
  l4_threadid_t myid, childid, thrdid, printerid, invid;
  Dit_Dhdr *dhdr;
  Dit_Phdr *phdr;
  char *c;
  int r,j,nmap;
  unsigned long msec, i;
  void *child_ip, *grandchild_ip, *child_base;
  dword_t **child_sp;
/****************************************************************************
 * first figure out my thread id and then ipc a hello message to serial
 * driver
 ****************************************************************************/ 

  /* point c at start of 64 byte register message buffer */
  c = (char *) &msg.reg[0]; 

  myid = l4_myself();
  kprintf("main:\tMapping tester\t0x%llx\n", myid.ID);
  kprintf("main:\tThis test will take a while, please be patient.\n");
  
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
  
  
  /* now loop through dit headers and find the child and grandchild */
  
  dhdr = (Dit_Dhdr *) (k->dit_hdr);

  child_ip = grandchild_ip = 0;
  for (phdr = (Dit_Phdr *) ((dword_t) dhdr +  dhdr->d_phoff), i = 0;
       i < dhdr->d_phnum;
       phdr++, i++)
  {
    kprintf("main:\t%-12s addr 0x%-8lx size 0x%-8lx\n",
	    phdr->p_name, phdr->p_base, phdr->p_size);
    if (phdr->p_flags == 0)
    {
      /* Not an initial server */
      if (!child_ip) {
	child_ip = (void *)(unsigned long)(phdr->p_entry);
	child_base = (void *)(unsigned long)(phdr->p_base);
	kprintf("main:\tchild %-12s addr 0x%-8lx", phdr->p_name, phdr->p_base);
	kprintf(" size 0x%-8lx entry 0x%-8lx\n", phdr->p_size, phdr->p_entry);
	assert(phdr->p_base % CHILD_AREA_SIZE == 0);
      } else {
	grandchild_ip = (void *)(unsigned long)(phdr->p_entry);
	kprintf("main:\tgrandchild %-12s addr 0x%-8lx",
		phdr->p_name, phdr->p_base);
	kprintf(" size 0x%-8lx entry 0x%-8lx\n", phdr->p_size, phdr->p_entry);
	assert((phdr->p_base+phdr->p_size-(dword_t)child_base) < CHILD_AREA_SIZE);
	break;
      }
    }
  }

  assert (child_ip);
#if 0
  assert (grandchild_ip);
#endif

  /* Claim all memory in child area */
  for (i=0; i<CHILD_AREA_SIZE/L4_PAGESIZE; i++) {
    j += *((unsigned char *) (child_base + i*L4_PAGESIZE));
    kprintf("main:\tgot  0x%-8lx\n", child_base + i*L4_PAGESIZE);
  }
  kprintf("main:\tclaimed all child memory\n");


  /* start printer thread for child stdout */
  printerid = myid;
  printerid.id.lthread++;
  invid.ID = -1LL;
  l4_thread_ex_regs(printerid,
		    (dword_t) printer,
		    (dword_t) &printer_stack[STACK_SIZE],
		    &invid, &invid, (dword_t*) &invid, (dword_t*) &invid);
  kprintf("main:\tstarted printer thread 0x%llx\n", printerid.ID);


  /* Push printer ID, grandchild start address and grandchild stack pointer
     to child stack */
  child_sp = (dword_t **)(((void *)child_base) + CHILD_AREA_SIZE);
  *(--child_sp) = (dword_t *)printerid.ID;
  *(--child_sp) = grandchild_ip;
  *(--child_sp) = (dword_t *)(((void *)child_base)+CHILD_AREA_SIZE-L4_PAGESIZE);
  kprintf("main:\tchild stack 0x%-8lx 0x%-8lx 0x%-8lx\n",
	  *child_sp, *(child_sp+1), *(child_sp+2));
  *(--child_sp) = (dword_t *)(dword_t) j; /* to fool optimiser */

  /* Start child. */
  childid = myid;
  childid.id.task++;
  childid = l4_task_new (childid, 0, (dword_t)child_sp, (dword_t)child_ip,
			 myid, myid);
  assert(childid.ID);
  kprintf("main:\tstarted child, id=0x%-8lx\n", childid.ID);

  /* Wait for child page fault */
  r = l4_mips_ipc_wait(&thrdid, L4_IPC_SHORT_MSG, &msg, L4_IPC_NEVER, &result);
  kprintf("main:\tpage fault 0x%-8lx 0x%-8lx\n", msg.reg[0], msg.reg[1]);
  assert(!r);
  assert(thrdid.ID == childid.ID);
  assert ((msg.reg[0] & (~(dword_t) 3)) == (dword_t)child_ip);
  assert (msg.reg[1] == (dword_t)child_ip);
  /* It's the page fault msg, map the whole child area at once */
  msg.reg[1] = l4_fpage((dword_t)child_base, CHILD_AREA_LOG_SIZE, 1, 0).fpage;
  msg.reg[2] = msg.reg[3] = 0;
  kprintf("main:\tsending fpage \t0x%llx 0x%llx\n", msg.reg[0], msg.reg[1]);

  r = l4_mips_ipc_reply_and_wait(childid, L4_IPC_SHORT_FPAGE, &msg,
				 &thrdid, L4_IPC_SHORT_MSG, &msg1,
				 L4_IPC_NEVER, &result);

  /* Now populate the child's whole address space with aliases of the same */
  kprintf("main:\tSending mappings, offset=%lx...\n", CHILD_AREA_SIZE*SPARSITY);
  msec=k->clock;
#ifndef MIN_MAP
  nmap = (1ll << (VA_SIZE-CHILD_AREA_LOG_SIZE))/SPARSITY;
#else
  nmap = 1;
#endif
  for (i=0; i<nmap; i++) {
    assert(!r);
    assert(thrdid.ID == childid.ID);
    if (msg1.reg[0] != -1ll  ||  msg1.reg[1] != -1ll) {
      kprintf("main:\tBase=0x%llx, reg0=0x%llx, reg1=0x%llx\n",
	      i*SPARSITY*CHILD_AREA_SIZE, msg1.reg[0], msg1.reg[1]);
      kprintf("main:\tSent 0x%llx 0x%llx\n", msg.reg[0], msg.reg[1]);
      l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
      assert(0);
    }
    msg.reg[0] = i*SPARSITY*CHILD_AREA_SIZE;
    if (i*SPARSITY*CHILD_AREA_SIZE != (dword_t)child_base) {
      r = l4_mips_ipc_reply_and_wait(childid, L4_IPC_SHORT_FPAGE, &msg,
				     &thrdid, L4_IPC_SHORT_MSG, &msg1,
				     L4_IPC_NEVER, &result);
    }
  }
  msec = k->clock - msec;
  assert(!r);
  assert(thrdid.ID == childid.ID);
  assert(msg1.reg[0] == -1ll);
  assert(msg1.reg[1] == -1ll);
  kprintf("main:\tdone\n");
  kprintf("main:\tSent %d mappings, %ld.%03ld sec (%d usec/mapping)\n",
	  nmap, msec/1000, msec%1000, (1000*msec)/nmap);

  kprintf("\nmain:\tChild's tests will take a few minutes...\n\n");

  /* Tell child that's it */
  r = l4_mips_ipc_send (childid, L4_IPC_SHORT_MSG, &msg, L4_IPC_NEVER, &result);
  assert(!r);

  /* Wait for child to finish */
  r = l4_mips_ipc_wait(&thrdid, L4_IPC_SHORT_MSG, &msg1, L4_IPC_NEVER, &result);
  if (msg1.reg[0]!=(dword_t)-2ll  || msg1.reg[1]!=(dword_t)-2ll) {
    kprintf("main:\tUnexpected child msg  \t0x%llx 0x%llx\n",
	    msg1.reg[0], msg1.reg[1]);
  }

  kprintf("main:\tMAPPING TEST COMPLETED\n");
  /* Sleep for one second */
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  assert(r == L4_IPC_RETIMEOUT);
  assert(0);
}
