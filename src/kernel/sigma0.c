/****************************************************************************
 *      $Id: sigma0.c,v 1.28 1999/09/09 03:17:30 gernot Exp $
 *      Copyright (C) 1997, 1998 Kevin Elphinstone, Univeristy of New South
 *      Wales.
 *
 *      This file is part of the L4/MIPS micro-kernel distribution.
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version 2
 *      of the License, or (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *      
 ****************************************************************************/
#include <kernel/panic.h>
#include <kernel/kmem.h>
#include <kernel/vm.h>
#include <r4kc0.h>
#include <kernel/trace.h>
#include <kernel/kutils.h>
#include <kernel/kernel.h>
#include <kernel/dit.h>
#include <l4/types.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>


static  Dit_Dhdr *dhdr;
static l4_kernel_info *kinfo;

#define MAX_PHYS_RAM (128*1024*1024)

#define USER_CACHE_SETTING EL_Cache_CNWb

void sigma0_init(void *used_so_far, udw_t ram_size)
{
  udw_t p;
  udw_t kernel_mem_start;

  /* init kernel frame list */

  kinfo = (l4_kernel_info *) (CKSEG0 + KERNEL_INFO_PAGE);
  kinfo->magic = 'L' << 24 | '4' << 16 | 'u' << 8 | 'K';
  kinfo->version = l4_version;
  kinfo->build = l4_build;
  kinfo->clock = 0;
  kinfo->memory_size = ram_size;
  /**** find end of downloaded binary to find start of free RAM ****/
  
  /* remove upper bits of address */
  used_so_far = (void *)((udw_t) used_so_far & (MAX_PHYS_RAM - 1));
  /* round up to nearest page boundary */
  used_so_far = (void *)(((udw_t)used_so_far | (L4_PAGESIZE -1)) + 1);

    
  dhdr = (Dit_Dhdr *)(CKSEG0 + (udw_t)used_so_far + INDY_OFFSET);
  if ( dhdr->d_ident[0] != 'd' ||
       dhdr->d_ident[1] != 'h' ||
       dhdr->d_ident[2] != 'd' ||
       dhdr->d_ident[3] != 'r')
  {
    panic("L4 PANIC: Sigma0 can't find DIT header"); /* ok */
  }
  
  kinfo->dit_hdr =  (dword_t) used_so_far;
  kinfo->kernel =  (dword_t) used_so_far + L4_PAGESIZE;
  
  
    
  /* reserve upper part of memory for kernel */

  kernel_mem_start = (ram_size - (ram_size / KERNEL_MEM_FRAC ))
    & ~(L4_PAGESIZE-1);
  kinfo->kernel_data = kernel_mem_start;

#if defined(IPT_128) || defined(IPT_8) || defined(CPT_128) || defined(CPT_8)
  {
#  if defined(IPT_128) || defined(IPT_8)   
#  include "vm/vm-ipt/ipt.h"
    for (p = ram_size - (IPT_INITIAL_SIZE);
	 p >= ram_size - (IPT_POOL_SIZE * IPT_INITIAL_SIZE);
	 p -= (IPT_INITIAL_SIZE))
    {
      ipt_head_free((void *)PHYS_TO_CKSEG0(p));
    }
#else
# include "vm/vm-cpt/ipt.h"
    for (p = ram_size - (CPT_INITIAL_SIZE);
	 p >= ram_size - (CPT_POOL_SIZE * CPT_INITIAL_SIZE);
	 p -= (CPT_INITIAL_SIZE))
    {
      cpt_head_free((void *)PHYS_TO_CKSEG0(p));
    }
#endif      
    for (
# ifdef INDY
	 p = kernel_mem_start + 0x08000000;
	 p < ram_size + 0x08000000;
# else
	 p = kernel_mem_start;
#if 	defined(IPT_128) || defined(IPT_8)  
	 p < ram_size - (IPT_POOL_SIZE * IPT_INITIAL_SIZE);
#else
	 p < ram_size - (CPT_POOL_SIZE * CPT_INITIAL_SIZE);
#endif
# endif
	 p += L4_PAGESIZE)
    {
      k_frame_free((void *)PHYS_TO_CKSEG0(p));
    }
  }
#else
  
  
  /* mark kernel memory used */
  for (
# ifdef INDY
       p = kernel_mem_start + 0x08000000;
       p < ram_size + 0x08000000;
# else
       p = kernel_mem_start;
       p < ram_size;
# endif
       p += L4_PAGESIZE)
  {
    k_frame_free((void *)PHYS_TO_CKSEG0(p));
  }
#endif
}



l4_msgdope_t s0_t_result, excpt_result;
int s0_t_r, excpt_r;
l4_ipc_reg_msg_t s0_t_msg, excpt_msg;
kernel_vars *s0_t_k;
l4_threadid_t s0_t_tid, excpt_tid;
dword_t s0_t_pte;

extern char outbuff[];
void printbuff(void);

char *excptname[] = {"Interrupt",                        /* 0 */
		     "TLB mod",                          
		     "TLB load",
		     "TLB store",
		     "Address Error (load or fetch)",    /* 4 */
		     "Address Error (store)",
		     "Bus Error (fetch)",
		     "Bus Error (data)",
		     "Syscall",                          /* 8 */  
		     "Break Point",
		     "Reserved Instruction",
		     "Coprocessor Unusable",
		     "Arithmetic Overflow",              /* 12 */
		     "Trap",
		     "Virtual Coherency (instruction)",
		     "Floating Point",
		     "Reserved??",                       /* 16 */
		     "Reserved??",
		     "Reserved??",
		     "Reserved??",
		     "Reserved??",                       /* 20 */
		     "Reserved??",
		     "Reserved??",
		     "Watch",
		     "Reserved??",                       /* 24 */
		     "Reserved??",
		     "Reserved??",
		     "Reserved??",
		     "Reserved??",                       /* 28 */ 
		     "Reserved??"
		     "Reserved??"
		     "Virtual Coherency (data)"
};

void sigma0_excpt_thread(void)
{
  excpt_r = l4_mips_ipc_wait(&excpt_tid, L4_IPC_SHORT_MSG, &excpt_msg,
			     L4_IPC_NEVER, &excpt_result);
  
  ints_off();
  sprintf(outbuff, "\r\nSIGMA0 EXCEPTION HANDLER INVOKED!!\r\n");
  printbuff();
  sprintf(outbuff, "task id 0x%llx took a '%s' exception\r\n",
	  excpt_tid.ID, excptname[excpt_msg.reg[0] >> 2 & 037]);
  printbuff();
  sprintf(outbuff, "EPC reg = 0x%llx, BVA reg = 0x%llx\r\n     ",
	  excpt_msg.reg[1], excpt_msg.reg[2]);
  printbuff();
  
  panic("");	/* FIXME, should shoot down task */
}
#define MSG_BUF_SIZE 256
char s0_panic_msg_buffer[MSG_BUF_SIZE];

pte_t *leaf;

void sigma0_thread(void)
{

  s0_t_k = KERNEL_BASE;
  
  while (1)
  {
    s0_t_r = l4_mips_ipc_wait(&s0_t_tid, L4_IPC_SHORT_MSG, &s0_t_msg,
			      L4_IPC_NEVER, &s0_t_result);
    if (s0_t_r == 0)
    {
      /* received valid message */

      if (s0_t_msg.reg[0] == SIGMA0_KERNEL_INFO_MAP)
      {
	s0_t_msg.reg[0] = (dword_t) KERNEL_INFO_PAGE;
	leaf = vm_lookup_pte(s0_t_k->gpt_pointer,
				  s0_t_msg.reg[0] >> L4_LOG2_PAGESIZE
				  << L4_LOG2_PAGESIZE);

	if (leaf == 0 || (!(*leaf & EL_Valid)))
	{
	  s0_t_pte = s0_t_msg.reg[0] >> L4_LOG2_PAGESIZE << 6 |
	    USER_CACHE_SETTING |  EL_Valid;
	  vm_sigz_insert_pte(s0_t_msg.reg[0], s0_t_pte);
	}
	s0_t_msg.reg[1] = (l4_fpage(s0_t_msg.reg[0] >> L4_LOG2_PAGESIZE
				    << L4_LOG2_PAGESIZE, L4_LOG2_PAGESIZE,
				    0,  L4_FPAGE_MAP)).fpage;
	s0_t_msg.reg[3] = 0;
	s0_t_r = l4_mips_ipc_send(s0_t_tid, L4_IPC_SHORT_FPAGE,
				  &s0_t_msg, L4_IPC_NEVER, &s0_t_result);
	if (s0_t_r != 0)
	{
	  panic("L4 PANIC: Sigma0 send of kernel info page failed"); /* ok */
	}
      }
      else if (s0_t_msg.reg[0] == SIGMA0_DEV_MAP)
      {
	/* devices and uncached ram are currently mapped as needed */
	 if ((s0_t_msg.reg[1] > kinfo->memory_size) ||
	     (s0_t_msg.reg[1] < kinfo->kernel_data &&
               s0_t_msg.reg[1] >= kinfo->kernel))
	{
	  leaf = vm_lookup_pte(s0_t_k->gpt_pointer,
			       s0_t_msg.reg[1] >> L4_LOG2_PAGESIZE
				    << L4_LOG2_PAGESIZE);

	  if (leaf == 0 || (!(*leaf & EL_Valid)))
	  {
	    s0_t_pte = s0_t_msg.reg[1] >> L4_LOG2_PAGESIZE << 6 |
	      EL_Cache_UC |  EL_Valid | EL_Dirty;
	    vm_sigz_insert_pte(s0_t_msg.reg[1], s0_t_pte);
	  }
	  s0_t_msg.reg[0] = s0_t_msg.reg[1];
	  s0_t_msg.reg[1] = (l4_fpage(s0_t_msg.reg[1] >> L4_LOG2_PAGESIZE
				      << L4_LOG2_PAGESIZE, L4_LOG2_PAGESIZE,
				      L4_FPAGE_RW,  L4_FPAGE_MAP)).fpage;
	  s0_t_msg.reg[3] = 0;
	  s0_t_r = l4_mips_ipc_send(s0_t_tid, L4_IPC_SHORT_FPAGE,
				    &s0_t_msg, L4_IPC_NEVER, &s0_t_result);
	  if (s0_t_r != 0)
	  {
	    panic("L4 PANIC: Sigma0 send of device mapping failed"); /* ok */
	  }
	}
      }
      else if ((s0_t_msg.reg[0] >> L4_LOG2_PAGESIZE << L4_LOG2_PAGESIZE)
	       == kinfo->dit_hdr)
      {
	
	s0_t_msg.reg[0] = (dword_t) kinfo->dit_hdr;
	leaf = vm_lookup_pte(s0_t_k->gpt_pointer,
			     s0_t_msg.reg[0] >> L4_LOG2_PAGESIZE
			     << L4_LOG2_PAGESIZE);
	if (leaf == 0 || (!(*leaf & EL_Valid)))
	{
	  s0_t_pte = s0_t_msg.reg[0] >> L4_LOG2_PAGESIZE << 6 |
	     USER_CACHE_SETTING |  EL_Valid;
	  vm_sigz_insert_pte(s0_t_msg.reg[0], s0_t_pte);
	}
	s0_t_msg.reg[1] = (l4_fpage((s0_t_msg.reg[0]+INDY_OFFSET)
				    >> L4_LOG2_PAGESIZE
				    << L4_LOG2_PAGESIZE, L4_LOG2_PAGESIZE,
				    0,  L4_FPAGE_MAP)).fpage;
	s0_t_msg.reg[3] = 0;
	s0_t_r = l4_mips_ipc_send(s0_t_tid, L4_IPC_SHORT_FPAGE,
				  &s0_t_msg, L4_IPC_NEVER, &s0_t_result);
	if (s0_t_r != 0)
	{
	  panic("L4 PANIC: Sigma0 send of dit info page failed"); /* ok */
	}
      }
      else if (s0_t_msg.reg[0] < kinfo->kernel_data &&
	       s0_t_msg.reg[0] >= kinfo->kernel)
      {
	/* attempt to map normal ram */
	leaf =  vm_lookup_pte(s0_t_k->gpt_pointer,
			      s0_t_msg.reg[0] >> L4_LOG2_PAGESIZE
			      << L4_LOG2_PAGESIZE);

	if (leaf == 0 || (!(*leaf & EL_Valid)))
	{
	  s0_t_pte = (s0_t_msg.reg[0]
#ifdef INDY
		      + 0x08000000
#endif
		      ) >> L4_LOG2_PAGESIZE << 6 |
	    USER_CACHE_SETTING |  EL_Valid | EL_Dirty;
	  vm_sigz_insert_pte(s0_t_msg.reg[0], s0_t_pte);
	  
	  
	  /* build fpage descriptor */
	  
	  s0_t_msg.reg[1] = (l4_fpage(s0_t_msg.reg[0] >> L4_LOG2_PAGESIZE
				      << L4_LOG2_PAGESIZE, L4_LOG2_PAGESIZE,
				      L4_FPAGE_RW, L4_FPAGE_MAP)).fpage;
	  s0_t_msg.reg[3] = 0;
#if 0
	  ints_off();
	  sprintf(outbuff,
		  "SIGMA0 INFO: mapping request address 0x%llx\r\n",
		  s0_t_msg.reg[0] );
	  printbuff();
	  ints_on();
#endif	  
	  s0_t_r = l4_mips_ipc_send(s0_t_tid, L4_IPC_SHORT_FPAGE,
				    &s0_t_msg, L4_IPC_NEVER, &s0_t_result);
	  if (s0_t_r != 0)
	  {
	    panic("L4 PANIC: Sigma0 sending of memory mapping failed"); /* ok */
	  }
	}
	else
	{
	  /* page already mapped */
	  s0_t_msg.reg[0] = 0;
	  s0_t_r = l4_mips_ipc_send(s0_t_tid, L4_IPC_SHORT_MSG,
				    &s0_t_msg, L4_IPC_NEVER, &s0_t_result);
	}
      }
      else
      {
	ints_off();
	sprintf(outbuff,
		"SIGMA0 INFO: mapping request to sigma0 out of available memory range\r\nthread id 0x%llx, address 0x%llx, IP 0x%llx\r\n",
		s0_t_tid.ID, s0_t_msg.reg[0], s0_t_msg.reg[1] );
	printbuff();
	
	ints_on();
	/* page already mapped */
	s0_t_msg.reg[0] = 0;
	s0_t_r = l4_mips_ipc_send(s0_t_tid, L4_IPC_SHORT_MSG,
				  &s0_t_msg, L4_IPC_NEVER, &s0_t_result);
      }
    }
  }
}
