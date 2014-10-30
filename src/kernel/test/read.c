/****************************************************************************
 * $Id: read.c,v 1.1 1999/02/10 04:55:47 gernot Exp $
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
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <libc.h>
#include <assert.h>

#define STACK_SIZE 128
dword_t _stack[STACK_SIZE];
dword_t *_sp = &_stack[STACK_SIZE];

static const l4_threadid_t SERIAL_TID = {0x1002000000060001L};

#define KBDSTACKSIZE 128
dword_t kbd_stack[KBDSTACKSIZE];


#define IN_BUF_SIZ 1024
struct {	/* buffer tty input using simple concurrency control */
  unsigned in, out;
  char     buf[IN_BUF_SIZ];
} kybd_buf;


void kbd_reader(void) {
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t     result;
  unsigned         r;
  l4_threadid_t    tid;
  char             ch;

  /* register myself as keyboard reader: */
  msg.reg[0] = 0;
  msg.reg[1] = (l4_myself()).ID;
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert (!r);

  while (1) {	/* wait for input and deposit in buffer */
    r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
    assert(!r);
    ch = msg.reg[0];

    while ((kybd_buf.in+1)%IN_BUF_SIZ == kybd_buf.out) {
      l4_thread_switch(L4_NIL_ID);	/* wait for buffer space */
    }
    kybd_buf.buf[kybd_buf.in] = ch;
    kybd_buf.in = (kybd_buf.in+1) % IN_BUF_SIZ;
  }
}


char get_char() {
  char ch;

  while (kybd_buf.in == kybd_buf.out) {
    l4_thread_switch(L4_NIL_ID);
  }
  ch = kybd_buf.buf[kybd_buf.out];
  kybd_buf.out = (kybd_buf.out+1) % IN_BUF_SIZ;
  return ch;
}
      


void main()
{
  int r;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  char *c, ch;
  l4_threadid_t tid, excpt, page;
  dword_t oip, osp;
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "Read test: my thread id is 0x%lx\n",tid.ID);

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(!r);

  sprintf(c,"Starting keyboard reader...");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  kybd_buf.in   = 0;
  kybd_buf.out  = 0;
  tid.id.lthread++;
  excpt.ID = -1LL;
  page.ID = -1LL;
  l4_thread_ex_regs(tid,
		    (dword_t) kbd_reader,
		    (dword_t) &kbd_stack[KBDSTACKSIZE],
		    &excpt,
		    &page,
		    &oip,
		    &osp);
  sprintf(c,"ID = 0x%lx\n", tid.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  sprintf(c,"Testing starts now.\nThis program should simply echo what ");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  sprintf(c,"you type.\n^D terminates test.\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  while (1) {
    ch = get_char();
#ifdef _DEBUGGING
    sprintf(c,"\nread 0x%x\n", ch);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
#endif /* _DEBUGGING */
    if (ch == 4) {
      assert(!"Terminated by ^D.");
    }
    msg.reg[0] = ch;
    sprintf(c, "%c", ch);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
			 L4_IPC_NEVER, &result);
  }
}

