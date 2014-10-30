/****************************************************************************
 * $Id: map_child.c,v 1.4 1999/09/12 15:14:00 gernot Exp $
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
#include <l4/time.h>
#include <stdarg.h>
#include <string.h>

#define STACK_SIZE 128
dword_t printer_stack[STACK_SIZE];


l4_threadid_t printerid;
void printer(void) {
  int              r;
  l4_msgdope_t     result;
  l4_threadid_t    thrdid;
  l4_ipc_reg_msg_t msg;

  strcpy((char*)&msg, "child:\tPrinter ready\n");
  while (1) {
    r = l4_mips_ipc_reply_and_wait(printerid, L4_IPC_SHORT_MSG, &msg,
				   &thrdid, L4_IPC_SHORT_MSG, &msg,
				   L4_IPC_NEVER, &result);
    assert (!r);
  }
}

union kbuf
{
  dword_t align; /* ensure start of buffer is double word aligned */
  char buf[256];
} printbuf;

int printf(const char *fmt, ...)
{ /* Note: This will only print strings <=64b correctly! */
  va_list         ap;
  int             len, r;
  l4_msgdope_t result;

  va_start (ap, fmt);
  len = vsprintf (printbuf.buf, fmt, ap);
  va_end (ap);
  
  r = l4_mips_ipc_send(printerid, L4_IPC_SHORT_MSG,
		       (l4_ipc_reg_msg_t *) printbuf.buf,
		       L4_IPC_NEVER, &result);
  assert(r == 0);
  return len;
}

main() {
  dword_t          **bottom;
  volatile dword_t *gch_ip, *gch_sp;
  dword_t	   step, base, max, start;
  l4_threadid_t    myid, nullid, parentid, gchid, gchprinterid, invid, thrdid;
  int              r, i, size, nmap;
  long		   junk;
  l4_msgdope_t     result;
  l4_ipc_reg_msg_t msg, msg1;
  l4_snd_fpage_t   sfp;

  bottom = (dword_t **) ((((dword_t)(&bottom))&L4_PAGEMASK)+L4_PAGESIZE);
  printerid.ID = (dword_t) *(bottom-1);
  gch_ip = (dword_t *) *(bottom-2);
  gch_sp = (dword_t *) *(bottom-3);
  parentid.ID = printerid.ID;
  parentid.id.lthread = 0;

  nullid.ID = 0;
  myid = l4_myself();
  printf("child:\tI'm 0x%-8lx\n", myid.ID);

  i = l4_id_nearest(SIGMA0_TID, &thrdid);      /* Might as well test this too */
  printf("child:\tparent %lx id_n %lx %x\n", parentid.ID, thrdid.ID, i);
  assert (parentid.ID == thrdid.ID);
  assert (i == L4_NC_OUTER_CLAN);

  printf("child:\tgrandchild sp=%lx ip=%lx\n", gch_sp, gch_ip);

  /* Get address space populated by parent */
  i=0;
  junk=0;
  msg.reg[0] = (dword_t)-1ll;
  msg.reg[1] = (dword_t)-1ll;
  sfp.fpage.fpage = 0;
  do {
    step = msg1.reg[0]&L4_PAGEMASK - sfp.snd_base&L4_PAGEMASK;
    sfp.snd_base    = msg1.reg[0];
    sfp.fpage.fpage = msg1.reg[1];
    r = l4_mips_ipc_call (parentid, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
			  &msg1, L4_IPC_NEVER, &result);
    assert (!r);
    junk += *(unsigned char *)msg1.reg[0];
    i++;
  } while (result.msgdope & L4_IPC_FPAGE_MASK);
  printf("child:\tGot %d mappings junk=%d\n", i-1, junk);/* fooling optimiser */


  /* start printer thread for grandchild stdout */
  gchprinterid = myid;
  gchprinterid.id.lthread++;
  invid.ID = -1LL;
  l4_thread_ex_regs(gchprinterid,
		    (dword_t) printer,
		    (dword_t) &printer_stack[STACK_SIZE],
		    &invid, &invid, (dword_t*) &invid, (dword_t*) &invid);
  printf("child:\tstarted printer thread 0x%llx\n", gchprinterid.ID);


  gchid = myid;
  gchid.id.task++;
  size  = sfp.fpage.fp.size;
  max   = sfp.snd_base+1l<<size;
  step  = step ? step : 1l<<size;
  base  = (dword_t)gch_ip;
  printf("child:\tSize=%d base=%lx max=%lx\n", size, base, max);
  *(--gch_sp) = gchprinterid.ID;
  for (; size<=64; size++) {
    /* set up grandchild stack */

    base = (size==64) ? 0l : base & ~((1l<<size)-1);
    printf("\nchild:\tTesting fpage size %d base %lx step %lx\n",
	   size, base, step);
    printf("child:\tStarting grandchild, ip=%lx sp=%lx\n", gch_ip, gch_sp);
    gchid = l4_task_new(gchid, 0, (dword_t)gch_sp, (dword_t)gch_ip, myid, myid);
    assert (gchid.ID);
    printf("child:\tstarted grandchild, id=0x%-8lx\n", gchid.ID);


    /* Wait for grandchild page fault */
    r = l4_mips_ipc_wait(&thrdid, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    printf("child:\tpage fault 0x%-8lx 0x%-8lx\n", msg.reg[0], msg.reg[1]);
    assert (!r);
    assert (thrdid.ID == gchid.ID);
    assert ((msg.reg[0] & (~(dword_t) 3)) == (dword_t)gch_ip);
    assert (msg.reg[1] == (dword_t)gch_ip);
    /* It's the page fault msg, map the whole child area at once */
    msg.reg[1] = l4_fpage(base, size, 1, 0).fpage;
    //msg.reg[1] = l4_fpage(msg.reg[0] & (~(dword_t) 3), 12, 1, 0).fpage;
    msg.reg[2] = msg.reg[3] = 0;
    printf("child:\tsending fpage \t0x%llx 0x%llx\n", msg.reg[0], msg.reg[1]);

    r = l4_mips_ipc_reply_and_wait(gchid, L4_IPC_SHORT_FPAGE, &msg,
				   &thrdid, L4_IPC_SHORT_MSG, &msg1,
				   L4_IPC_NEVER, &result);

    /* Now populate the grandchild's address space */
    printf("child:\tSending mappings, this may take a while...\n");
    for (i=0,nmap=0; (start=i*(1l<<size-1))<max/2; i++) {
      start *= 2;
      assert(!r);
      assert(thrdid.ID == gchid.ID);
      if (msg1.reg[0] != -1ll  ||  msg1.reg[1] != -1ll) {
	printf("child:\tUnexpected page fault - maybe maping problem?\n");
	printf("child:\tBase=%llx, reg0=%llx, reg1=%llx\n",
		start, msg1.reg[0], msg1.reg[1]);
	printf("child:\tLast sent %llx %llx\n", msg.reg[0], msg.reg[1]);
	l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
	assert(0);
      }
      msg.reg[0] = start + !(start%step);	/* odd means a real mapping */
      msg.reg[1] = l4_fpage(start, size, 1, 0).fpage;
      if (start != (dword_t)base) {
	r = l4_mips_ipc_reply_and_wait(gchid, L4_IPC_SHORT_FPAGE, &msg,
				       &thrdid, L4_IPC_SHORT_MSG, &msg1,
				       L4_IPC_NEVER, &result);
	nmap++;
      }
    }
    assert(!r);
    assert(thrdid.ID == gchid.ID);
    assert(msg1.reg[0] == -1ll);
    assert(msg1.reg[1] == -1ll);
    printf("child:\tdone\n");
    printf("child:\tSent %d mappings of size %d\n", nmap, size);

    /* Tell grandchild that's it */
    r = l4_mips_ipc_send (gchid, L4_IPC_SHORT_MSG, &msg, L4_IPC_NEVER, &result);
    assert(!r);

    /* Wait for grandchild to finish */
    r = l4_mips_ipc_wait(&thrdid, L4_IPC_SHORT_MSG, &msg1,
			 L4_IPC_NEVER, &result);
    if (msg1.reg[0]!=(dword_t)-2ll  || msg1.reg[1]!=(dword_t)-2ll) {
      printf("child:\tUnexpected msg 0x%llx 0x%llx\n",
	     msg1.reg[0], msg1.reg[1]);
    }

    l4_mips_sleep_msecs (200, L4_T_ROUND_NEAREST);
    printf("child:\tkilling grandchild\n");
    gchid = l4_task_new(gchid, myid.ID, 0l, 0l, nullid, myid);
    assert (gchid.ID);
  }

  printf("child:\tI'm done.\n");

  /* Tell parent it's all over... */
  msg.reg[0] = (dword_t)-2ll;
  msg.reg[1] = (dword_t)-2ll;
  r = l4_mips_ipc_call (parentid, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_SHORT_MSG, &msg1, L4_IPC_NEVER, &result);
  assert(0);	/* Shouldn't get here... */
}
