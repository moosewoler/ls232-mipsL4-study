/****************************************************************************
 * $Id: map_gchild.c,v 1.2 1999/09/12 02:53:25 gernot Exp $
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
#include <stdarg.h>


union kbuf
{
  dword_t align; /* ensure start of buffer is double word aligned */
  char buf[256];
} printbuf;

l4_threadid_t printerid;
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
  void             *gch_ip, *gch_sp;
  int              r, i, j;
  long		   junk;
  l4_msgdope_t     result;
  l4_ipc_reg_msg_t msg, msg1;
  l4_threadid_t    myid, nullid, parentid, thrdid;

  bottom = (dword_t **) ((((dword_t)(&bottom))&L4_PAGEMASK)+L4_PAGESIZE);
  printerid.ID = (dword_t) *(bottom-1);
  parentid.ID = printerid.ID;
  parentid.id.lthread = 0;

  myid = l4_myself();
  printf("grandchild:\tI'm 0x%-8lx\n", myid.ID);

  i = l4_id_nearest(SIGMA0_TID, &thrdid);
  printf("grandchild:\tparent %lx id_n %lx %x\n", parentid.ID, thrdid.ID, i);
  assert (parentid.ID == thrdid.ID);
  assert (i == L4_NC_OUTER_CLAN);

  /* Get address space populated by parent */
  i=0;
  j=0;
  junk=0;
  msg.reg[0] = (dword_t)-1ll;
  msg.reg[1] = (dword_t)-1ll;
  do {
    r = l4_mips_ipc_call (parentid, L4_IPC_SHORT_MSG, &msg,
			  L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
			  &msg1, L4_IPC_NEVER, &result);
    assert (!r);
    if (msg1.reg[0]%2) {	/* should be a real mapping */
      junk += *(unsigned char *)msg1.reg[0];
      j++;
    }
    i++;
  } while (result.msgdope & L4_IPC_FPAGE_MASK);
  printf("grandchild:\tGot %d mappings, %d real, junk=%d\n",
	 i, j, junk);	/* fooling optimiser */

  /* l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result); */
  printf("grandchild:\tI'm done.\n");

  /* Tell parent it's all over... */
  msg.reg[0] = (dword_t)-2ll;
  msg.reg[1] = (dword_t)-2ll;
  r = l4_mips_ipc_call (parentid, L4_IPC_SHORT_MSG, &msg,
			L4_IPC_SHORT_MSG, &msg1, L4_IPC_NEVER, &result);
  assert(0);	/* Shouldn't get here... */
}
