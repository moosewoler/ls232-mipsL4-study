/****************************************************************************
 * $Id: time.c,v 1.4 1999/02/10 05:25:03 gernot Exp $
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
#include <l4/time.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
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
      


#define printf(f...) \
  { sprintf((char*)&msg, ## f); \
    l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg, \
		     L4_IPC_NEVER, &result); }

#define FAILMSG(in,d,m,e,out,should) \
  printf("FAILED:\n0x%lx ms, ", (in)); \
  printf("%s, m = %d, e = %d -> 0x%lx, expected 0x%lx\n", \
	  (d), (m), (e), (out), (should)); \
  if (fail++ > 7) { \
    l4_mips_sleep_msecs(100, L4_T_ROUND_NEAREST); \
    assert(!"Tests failed"); \
  }

#define FAILPFMSG(in,d,e,out,should) \
  printf("FAILED:\n0x%lx ms, ", (in)); \
  printf("PF %s, e = %d -> 0x%lx, expected 0x%lx\n", \
	  (d), (e), (out), (should)); \
  if (fail++ > 7) { \
    l4_mips_sleep_msecs(100, L4_T_ROUND_NEAREST); \
    assert(!"Tests failed"); \
  }

#define TEST_DOWN(in, should, tol) \
  l4_mips_encode_timeout((in), &mant, &exp, L4_T_ROUND_DOWN); \
  res=l4_mips_decode_timeout(mant, exp); \
  if (res != (should) && (res+(tol) < (should)  ||  res > (should)+(tol))) { \
    FAILMSG(in, "DOWN", mant, exp, res, (should)); \
  }

#define TEST_NEAREST(in, should, tol) \
  l4_mips_encode_timeout((in), &mant, &exp, L4_T_ROUND_NEAREST); \
  res=l4_mips_decode_timeout(mant, exp); \
  if (res != (should) && (res+(tol) < (should)  ||  res > (should)+(tol))) { \
    FAILMSG(in, "NEAREST", mant, exp, res, (should)); \
  }

#define TEST_UP(in, should, tol) \
  l4_mips_encode_timeout((in), &mant, &exp, L4_T_ROUND_UP); \
  res=l4_mips_decode_timeout(mant, exp); \
  if (res != (should) && (res+(tol) < (should)  ||  res > (should)+(tol))) { \
    FAILMSG(in, "UP", mant, exp, res, (should)); \
  }

#define TEST_PF_DOWN(in, should, tol) \
  l4_mips_encode_pf_timeout((in), &exp, L4_T_ROUND_DOWN); \
  res=l4_mips_decode_pf_timeout(exp); \
  if (res != (should) && (res+(tol) < (should)  ||  res > (should)+(tol))) { \
    FAILPFMSG(in, "DOWN", exp, res, (should)); \
  }

#define TEST_PF_NEAREST(in, should, tol) \
  l4_mips_encode_pf_timeout((in), &exp, L4_T_ROUND_NEAREST); \
  res=l4_mips_decode_pf_timeout(exp); \
  if (res != (should) && (res+(tol) < (should)  ||  res > (should)+(tol))) { \
    FAILPFMSG(in, "NEAREST", exp, res, (should)); \
  }

#define TEST_PF_UP(in, should, tol) \
  l4_mips_encode_pf_timeout((in), &exp, L4_T_ROUND_UP); \
  res=l4_mips_decode_pf_timeout(exp); \
  if (res != (should) && (res+(tol) < (should)  ||  res > (should)+(tol))) { \
    FAILPFMSG(in, "UP", exp, res, (should)); \
  }

void main()
{
  int r, fail=0, tol=0, hex;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  char *c, ch;
  l4_threadid_t tid, excpt, page;
  byte_t mant, exp;
  dword_t ms, res, i=0, j, oip, osp;
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "Time test: my thread id is 0x%lx\n",tid.ID);

  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  assert(!r);

  printf("Starting keyboard reader...");
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
  printf("ID = 0x%lx\n", tid.ID);

  printf("Testing timeout encodings...");

  tol=0;
  TEST_DOWN(0l, 0l, tol);
  TEST_NEAREST(0l, 0l, tol);
  TEST_UP(0l, 0l, tol);
  for (ms=1; ms<256; ms++) {
    if (ms==20) tol=1;
    TEST_DOWN(ms, ms-1, tol);
    TEST_NEAREST(ms, ms, tol);
    TEST_UP(ms, ms, tol);
  }
  tol = 4;
  for (ms=256; ms<1024; ms+=4) {
    TEST_DOWN(ms, ms, tol);
    TEST_NEAREST(ms, ms, tol);
    TEST_UP(ms, ms, tol);
  }
  for (i=1024, j=1; i<63*1024*1024l; i*=4) {
    tol = 16*j;
    TEST_DOWN(i, i, tol);
    TEST_NEAREST(i, i, tol);
    TEST_UP(i, i, tol);
    if (j < 64*1024*1024l) {
      j*=4;
    }
  }

  TEST_PF_DOWN(0l, 0l, 0);
  TEST_PF_NEAREST(0l, 0l, 0);
  TEST_PF_UP(0l, 0l, 0);
  TEST_PF_DOWN(1l, 1l, 0);
  TEST_PF_NEAREST(1l, 1l, 0);
  TEST_PF_UP(1l, 1l, 0);
  TEST_PF_DOWN(2l, 1l, 0);
  TEST_PF_NEAREST(2l, 4l, 0);
  TEST_PF_UP(2l, 4l, 0);
  TEST_PF_DOWN(2l, 1l, 0);
  TEST_PF_NEAREST(3l, 4l, 0);
  TEST_PF_UP(3l, 4l, 0);
  tol = 0;
  for (j=1; j<=64*1024; j*=4) {
    TEST_PF_DOWN(4*j, 4*j, j/4);
    TEST_PF_NEAREST(4*j, 4*j, j/4);
    TEST_PF_UP(4*j, 4*j, j/4);
    tol = 4*j;
    for (i=5; i<0x10; i++) {
      ms=i*j;
      TEST_PF_DOWN(ms, 4*j, tol);
      if (j==64*1024) {
	TEST_PF_NEAREST(ms, 4*j, tol);
	TEST_PF_UP(ms, ~(dword_t)0, tol);
      } else {
	TEST_PF_NEAREST(ms, i>=8 ? 0x10*j : 4*j, tol);
	TEST_PF_UP(ms, 0x10*j, tol);
      }
    }
  }

  if (!fail) {
    printf("PASSED!\n");
  }
  printf("End automatic tests, interactive tests follow.\n");
  printf("Enter times in decimal or hex, ^D to finish.\n");

  while (1) {
    printf("Time in msec: ");

    ms=0;
    hex=0;
    i=0;
    while (1) {
      ch = get_char();
      assert(ch!=4);	/* ^D to exit */
      if (ms==0 && !hex && ch=='x') {
	hex = 1;
      } else if (hex  &&  ('a' <= ch  &&  ch <= 'f')) {
	ms = 0x10*ms + ch-'a'+0xa;
      } else if ('0' <= ch  &&  ch <= '9') {
	ms = (hex?0x10:10)*ms + ch-'0';
      } else {
	break;
      }
      printf("%c", ch);
    }
    l4_mips_encode_timeout(ms, &mant, &exp, L4_T_ROUND_DOWN);
    res = l4_mips_decode_timeout(mant, exp);
    printf("\n%9ld ms,\tmant=%3d,\texp=%d,\tDOWN  %8ld ms\n",
	    ms, mant, exp, res);
    printf("%9lx ms,\tmant=%3x,\texp=%x,\tDOWN  %8lx ms\n",
	    ms, mant, exp, res);
    l4_mips_encode_timeout(ms, &mant, &exp, L4_T_ROUND_NEAREST);
    res = l4_mips_decode_timeout(mant, exp);
    printf("%9ld ms,\tmant=%3d,\texp=%d,\tNEAR  %8ld ms\n",
	    ms, mant, exp, res);
    printf("%9lx ms,\tmant=%3x,\texp=%x,\tNEAR  %8lx ms\n",
	    ms, mant, exp, res);
    l4_mips_encode_timeout(ms, &mant, &exp, L4_T_ROUND_UP);
    res = l4_mips_decode_timeout(mant, exp);
    printf("%9ld ms,\tmant=%3d,\texp=%d,\tUP    %8ld ms\n",
	    ms, mant, exp, res);
    printf("%9lx ms,\tmant=%3x,\texp=%x,\tUP    %8lx ms\n",
	    ms, mant, exp, res);
    l4_mips_encode_pf_timeout(ms, &exp, L4_T_ROUND_DOWN);
    res = l4_mips_decode_pf_timeout(exp);
    printf("%9ld ms,\t\t\texp=%d,\tPF-DOWN %6ld ms\n", ms, exp, res);
    printf("%9lx ms,\t\t\texp=%x,\tPF-DOWN %6lx ms\n", ms, exp, res);
    l4_mips_encode_pf_timeout(ms, &exp, L4_T_ROUND_NEAREST);
    res = l4_mips_decode_pf_timeout(exp);
    printf("%9ld ms,\t\t\texp=%d,\tPF-NEAR %6ld ms\n", ms, exp, res);
    printf("%9lx ms,\t\t\texp=%x,\tPF-NEAR %6lx ms\n", ms, exp, res);
    l4_mips_encode_pf_timeout(ms, &exp, L4_T_ROUND_UP);
    res = l4_mips_decode_pf_timeout(exp);
    printf("%9ld ms,\t\t\texp=%d,\tPF-UP   %6ld ms\n", ms, exp, res);
    printf("%9lx ms,\t\t\texp=%x,\tPF-UP   %6lx ms\n", ms, exp, res);
    l4_mips_sleep_msecs(100, L4_T_ROUND_NEAREST);
  }
}

