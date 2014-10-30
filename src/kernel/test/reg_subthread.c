/****************************************************************************
 * $Id: reg_subthread.c,v 1.4 1999/11/06 10:19:46 gernot Exp $
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

#define AT_NBR 1
#define	AT_VAL 0x010beef
#define K0_NO 26
#define K1_NO 27

int kprintf(const char *fmt, ...);

static
char *regname[] = {"zero", "AT", "v0", "v1", "a0", "a1", "a2", "a3",
		   "a4", "a5", "a6", "a7", "t0", "t1", "t2", "t3",
		   "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
		   "t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra",
		   "hi", "lo"};
static dword_t regs_orig[34], regs_bef[34], regs_aft[34];
#ifdef PAGE_FAULT
static dword_t free_pages[0x800];
static dword_t *fault_adr = &free_pages[0x400];
static char name[] = "page fault";
#else
static char name[] = "exception";
#endif

static int n_trash=0;

void excepter(void)
{
  l4_ipc_reg_msg_t msg;
  l4_msgdope_t result;
  l4_threadid_t myid, OSid;
  int r, count, i;

  myid = l4_myself();
  kprintf("%s thread:\tHello world, I'm\t0x%llx!\n", name, myid.ID);

  for (i = 0; i < 32; i++)
      regs_orig[i] = regs_bef[i] = regs_aft[i] = 0xab;

#ifdef PAGE_FAULT
  kprintf("%s thread:\tAbout to touch 0x%llx\n", name, fault_adr);
#else
  kprintf("%s thread:\tAbout to take an exception\n", name);
#endif

  regs_bef[AT_NBR] = AT_VAL;
  asm("sd $0, regs_orig;"
      "sd $2, regs_orig+16;"
      "sd $3, regs_orig+24;"
      "sd $4, regs_orig+32;"
      "sd $5, regs_orig+40;"
      "sd $6, regs_orig+48;"
      "sd $7, regs_orig+56;"
      "sd $8, regs_orig+64;"
      "sd $9, regs_orig+72;"
      "sd $10, regs_orig+80;"
      "sd $11, regs_orig+88;"
      "sd $12, regs_orig+96;"
      "sd $13, regs_orig+104;"
      "sd $14, regs_orig+112;"
      "sd $15, regs_orig+120;"
      "sd $16, regs_orig+128;"
      "sd $17, regs_orig+136;"
      "sd $18, regs_orig+144;"
      "sd $19, regs_orig+152;"
      "sd $20, regs_orig+160;"
      "sd $21, regs_orig+168;"
      "sd $22, regs_orig+176;"
      "sd $23, regs_orig+184;"
      "sd $24, regs_orig+192;"
      "sd $25, regs_orig+200;"
      "sd $26, regs_orig+208;"
      "sd $27, regs_orig+216;"
      "sd $28, regs_orig+224;"
      "sd $29, regs_orig+232;"
      "sd $30, regs_orig+240;"
      "sd $31, regs_orig+248;"
      "mfhi $4;"
      "mflo $5;"
      "sd $4, regs_orig+256;"
      "sd $5, regs_orig+264;"
      "dli $4, 0x320beef;"
      "dli $5, 0x330beef;"
      "mthi $4;"
      "mtlo $5;"
      "sd $4, regs_bef+256;"
      "sd $5, regs_bef+264;"
      "dli $0, 0x000beef;"
      "dli $2, 0x020beef;"
      "dli $3, 0x030beef;"
      "dli $4, 0x040beef;"
      "dli $5, 0x050beef;"
      "dli $6, 0x060beef;"
      "dli $7, 0x070beef;"
      "dli $8, 0x080beef;"
      "dli $9, 0x090beef;"
      "dli $10, 0x100beef;"
      "dli $11, 0x110beef;"
      "dli $12, 0x120beef;"
      "dli $13, 0x130beef;"
      "dli $14, 0x140beef;"
      "dli $15, 0x150beef;"
      "dli $16, 0x160beef;"
      "dli $17, 0x170beef;"
      "dli $18, 0x180beef;"
      "dli $19, 0x190beef;"
      "dli $20, 0x200beef;"
      "dli $21, 0x210beef;"
      "dli $22, 0x220beef;"
      "dli $23, 0x230beef;"
      "dli $24, 0x240beef;"
      "dli $25, 0x250beef;"
      "dli $26, 0x260beef;"
      "dli $27, 0x270beef;"
      "dli $28, 0x280beef;"
#ifdef PAGE_FAULT
      "ld $29, fault_adr;"
#else
      "dli $29, 0x290beef;"
#endif
      "dli $30, 0x300beef;"
      "dli $31, 0x310beef;"
      "sd $0, regs_bef;"
      "sd $2, regs_bef+16;"
      "sd $3, regs_bef+24;"
      "sd $4, regs_bef+32;"
      "sd $5, regs_bef+40;"
      "sd $6, regs_bef+48;"
      "sd $7, regs_bef+56;"
      "sd $8, regs_bef+64;"
      "sd $9, regs_bef+72;"
      "sd $10, regs_bef+80;"
      "sd $11, regs_bef+88;"
      "sd $12, regs_bef+96;"
      "sd $13, regs_bef+104;"
      "sd $14, regs_bef+112;"
      "sd $15, regs_bef+120;"
      "sd $16, regs_bef+128;"
      "sd $17, regs_bef+136;"
      "sd $18, regs_bef+144;"
      "sd $19, regs_bef+152;"
      "sd $20, regs_bef+160;"
      "sd $21, regs_bef+168;"
      "sd $22, regs_bef+176;"
      "sd $23, regs_bef+184;"
      "sd $24, regs_bef+192;"
      "sd $25, regs_bef+200;"
      "sd $26, regs_bef+208;"
      "sd $27, regs_bef+216;"
      "sd $28, regs_bef+224;"
      "sd $29, regs_bef+232;"
      "sd $30, regs_bef+240;"
      "sd $31, regs_bef+248;"
      "mfhi $4;"
      "mflo $5;"
      "sd $4, regs_bef+256;"
      "sd $5, regs_bef+264;"
      "ld $4, regs_bef+32;"
      "ld $5, regs_bef+40;"
      ".set noat;"
      "dli $1, 0x010beef;"
      "dadd $4, $4, $1;"	// to make up for subtraction of AT

#ifdef PAGE_FAULT
      "sd $0,($29);"		// cause page fault
#else
//    "mfc0 $4, $12;"		// cause CpU exception
      "sd $4, 1($0);"		// cause AdES exception
#endif

      "sub $4, $4, $1;"		// subtract AT from a0
      ".set at;"
      "sd $0, regs_aft;"
      "sd $2, regs_aft+16;"
      "sd $3, regs_aft+24;"
      "sd $4, regs_aft+32;"
      "sd $5, regs_aft+40;"
      "sd $6, regs_aft+48;"
      "sd $7, regs_aft+56;"
      "sd $8, regs_aft+64;"
      "sd $9, regs_aft+72;"
      "sd $10, regs_aft+80;"
      "sd $11, regs_aft+88;"
      "sd $12, regs_aft+96;"
      "sd $13, regs_aft+104;"
      "sd $14, regs_aft+112;"
      "sd $15, regs_aft+120;"
      "sd $16, regs_aft+128;"
      "sd $17, regs_aft+136;"
      "sd $18, regs_aft+144;"
      "sd $19, regs_aft+152;"
      "sd $20, regs_aft+160;"
      "sd $21, regs_aft+168;"
      "sd $22, regs_aft+176;"
      "sd $23, regs_aft+184;"
      "sd $24, regs_aft+192;"
      "sd $25, regs_aft+200;"
      "sd $26, regs_aft+208;"
      "sd $27, regs_aft+216;"
      "sd $28, regs_aft+224;"
      "sd $29, regs_aft+232;"
      "sd $30, regs_aft+240;"
      "sd $31, regs_aft+248;"
      "mfhi $4;"
      "mflo $5;"
      "sd $4, regs_aft+256;"
      "sd $5, regs_aft+264;"
      "ld $4, regs_orig+256;"
      "ld $5, regs_orig+264;"
      "mthi $4;"
      "mtlo $5;"
      "ld $0, regs_orig;"
      "ld $2, regs_orig+16;"
      "ld $3, regs_orig+24;"
      "ld $4, regs_orig+32;"
      "ld $5, regs_orig+40;"
      "ld $6, regs_orig+48;"
      "ld $7, regs_orig+56;"
      "ld $8, regs_orig+64;"
      "ld $9, regs_orig+72;"
      "ld $10, regs_orig+80;"
      "ld $11, regs_orig+88;"
      "ld $12, regs_orig+96;"
      "ld $13, regs_orig+104;"
      "ld $14, regs_orig+112;"
      "ld $15, regs_orig+120;"
      "ld $16, regs_orig+128;"
      "ld $17, regs_orig+136;"
      "ld $18, regs_orig+144;"
      "ld $19, regs_orig+152;"
      "ld $20, regs_orig+160;"
      "ld $21, regs_orig+168;"
      "ld $22, regs_orig+176;"
      "ld $23, regs_orig+184;"
      "ld $24, regs_orig+192;"
      "ld $25, regs_orig+200;"
      "ld $26, regs_orig+208;"
      "ld $27, regs_orig+216;"
      "ld $28, regs_orig+224;"
      "ld $29, regs_orig+232;"
      "ld $30, regs_orig+240;"
      "ld $31, regs_orig+248;"
      );

  for (i = 0; i < 34; i++) {
      kprintf("%s thread\t%s:\t0x%llx\t0x%llx", name,
	      regname[i], regs_bef[i], regs_aft[i]);
      if (regs_bef[i] != regs_aft[i] && (i!=AT_NBR) && (i!=K0_NO) && (i!=K1_NO)) {
	n_trash++;
	kprintf("\t<== TRASHED!");
	if (i==4) {
	  kprintf(" Could be AT or a0!");
	}
      }
      kprintf("\n");
  }
  if (n_trash) {
    kprintf("\n%s thread\t%d register(s) trashed!\n", name, n_trash);
  } else {
    kprintf("\n%s thread\tno registers trashed - test successful!\n", name);
  }
  kprintf("\n%s thread\t======== End of test ========\n", name);

  r = l4_mips_ipc_sleep(L4_IPC_NEVER,&result);
  assert(0);
}
