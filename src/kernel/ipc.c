/****************************************************************************
 *      $Id: ipc.c,v 1.12 1999/02/10 04:55:18 gernot Exp $
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
#include <kernel/kernel.h>
#include <kernel/vm.h>
#include <kernel/panic.h>
#include <l4/ipc.h>

dword_t do_long_ipc(dword_t sdesc,
		    dword_t rdesc,
		    dword_t *sregs,
		    tcb_t *stcb,
		    tcb_t *dtcb,
		    dword_t status)
{
  l4_msgdope_t r;
  dword_t window_addr;
  r.msgdope = status;
  window_addr = 0;

  /****************************************************************************
   *
   * set state correctly as we may be switch out due to page fault
   * or interrupts when enabled 
   *
   ****************************************************************************
   */
  
  stcb->fine_state = FS_LOCKS | FS_BUSY;
  dtcb->fine_state = FS_LOCKR ;

  stcb->comm_partner =  dtcb;
  dtcb->comm_partner =  stcb;
  

  /****************************************************************************
   * attempt fpage transfer
   ****************************************************************************
   */
#ifdef KDEBUG
  alphn_set("dli0");
#endif
  if (sdesc & L4_IPC_FPAGE_MASK)
  {
    /* we have fpage ipc */
    dword_t recv_fpage;
    
    recv_fpage = 0;
    
    /* find receiver fpage */
    if (rdesc & L4_IPC_FPAGE_MASK)
    {
      recv_fpage = rdesc;
    }
    else if (rdesc & (~(dword_t)(L4_IPC_FPAGE_MASK | L4_IPC_DECEIT_MASK)))
    {
      /* receive fpage in memory */
      if (rdesc <  USER_ADDR_TOP)
      {
	/* set up i/o window */
	window_addr = RECV_WINDOW_BASE +
	  (((stcb->myself & TID_THREAD_MASK) >> TID_THREAD_SHIFT)
	   * RECV_WINDOW_SIZE) +
	  (rdesc & (4 * 1024 * 1024 - 1 - 7));
	stcb->wdw_map_addr = rdesc & (~(dword_t)(4*1024*1024 -1));
	recv_fpage = *(dword_t *) window_addr;
      }
    }
    if (recv_fpage != 0)
    {
      int i;
      /* first do registers */
      for (i = 0; i < 4; i++)
      {
	if (sregs[i*2+1] != 0)
	{
#ifdef MAP_INSTR
	  udw_t start_time, end_time;
	  extern void get_cnt(udw_t *);
	  get_cnt(&start_time);
#endif

#ifdef KDEBUG
	  alphn_set("dlim");
#endif
	  
	  vm_map(stcb, sregs[i*2+1], sregs[i*2],
		 dtcb, recv_fpage);
#ifdef MAP_INSTR
	  get_cnt(&end_time);
	  {
	    kernel_vars *k = KERNEL_BASE;
	    if (k->tlb_miss != 0)
	    {
	      *(unsigned long *) (k->tlb_miss) = end_time - start_time;
	    }
	  }
#endif  
	  
	  r.md.fpage_received = 1;
#ifdef KDEBUG
	  alphn_set("dlam");
#endif
	}
	else
	{
	  recv_fpage = 0;
	  break;
	}
      }

      if ((sdesc & (~(dword_t)(L4_IPC_FPAGE_MASK | L4_IPC_DECEIT_MASK)))
	  && (recv_fpage != 0))
      {
	/* do fpages from memory */
	/* FIXME: implement */
      }
    }
  }
  /****************************************************************************
   * attempt direct string transfer
   ****************************************************************************
   */
  if (sdesc & (~(dword_t)(L4_IPC_FPAGE_MASK | L4_IPC_DECEIT_MASK)))
  {
    /* check at least snd and rcv header is in user address space */
    if ((sdesc + sizeof(l4_msghdr_t)) < USER_ADDR_TOP )
    {
      l4_msghdr_t *snd_hdr;
      snd_hdr = (l4_msghdr_t *) (sdesc & (~(dword_t) 7));
      /* check we are actually sending something before we go
	 further */
      if (snd_hdr -> snd_dope.md.dwords == 0 &&
	  snd_hdr -> snd_dope.md.strings == 0)
      {
	if (window_addr != 0)
	{
	  tlb_flush_window(window_addr);
	}
	return r.msgdope;
      }
      
      if (
	  ((rdesc & L4_IPC_FPAGE_MASK) == 0) &&
	  (rdesc & (~(dword_t)3)) &&
	  ((rdesc + sizeof(l4_msghdr_t)) < USER_ADDR_TOP))
      {
	l4_msghdr_t *rcv_hdr;

	/* set up io window to read rcv hdr */
	window_addr = RECV_WINDOW_BASE +
	  (((stcb->myself & TID_THREAD_MASK) >> TID_THREAD_SHIFT)
	   * RECV_WINDOW_SIZE) +
	  (rdesc & (4 * 1024 * 1024 - 1 - 7));
	stcb->wdw_map_addr = rdesc & (~(dword_t)(4*1024*1024 -1));

	/* note: 8 byte alignment forced for headers to avoid alignment
	   exceptions */
	rcv_hdr = (l4_msghdr_t *) window_addr;

	/* given headers we check boundary of message below top of user
	   address space */
	if (
	    ((sdesc + sizeof(l4_msghdr_t) +
	      8 * snd_hdr->size_dope.md.dwords +
	      sizeof(l4_strdope_t) * snd_hdr->size_dope.md.strings)
	     >=  USER_ADDR_TOP) ||
	    ((rdesc + sizeof(l4_msghdr_t) +
	      8 * rcv_hdr->size_dope.md.dwords +
	      sizeof(l4_strdope_t) * rcv_hdr->size_dope.md.strings)
	     >=  USER_ADDR_TOP) ||
	    (snd_hdr->snd_dope.md.dwords > snd_hdr->size_dope.md.dwords) 
	    )
	{
	  r.msgdope |= L4_IPC_REMSGCUT;
	  tlb_flush_window(window_addr);
	  return r.msgdope;
	} 
	/* transfer any dwords */
	if (snd_hdr -> snd_dope.md.dwords != 0)
	{
	  dword_t *sp,*rp;
	  int i;
	  if (rcv_hdr->size_dope.md.dwords < snd_hdr -> snd_dope.md.dwords)
	  {
	    r.msgdope |= L4_IPC_REMSGCUT;
	    tlb_flush_window(window_addr);
	    return r.msgdope;
	  }

	  sp = (dword_t *)((char *) snd_hdr + sizeof(l4_msghdr_t));
	  rp = (dword_t *)((char *) rcv_hdr + sizeof(l4_msghdr_t));
          ints_on();
	  for (i = 0; i < snd_hdr ->snd_dope.md.dwords; i++)
	  {
	    rp[i] = sp[i];
	  }
	  ints_off();
	  r.md.dwords = snd_hdr->snd_dope.md.dwords;
	}
	/**************************************************************
	 * transfer any indirect strings 
	 **************************************************************
	 */
	if (snd_hdr -> snd_dope.md.strings != 0)
	{
	  int i;
	  dword_t direct_map_addr;
	  l4_strdope_t *snd_strings, *rcv_strings;

	  /* check receiver willing to accept sender strings */
	  if ((snd_hdr ->snd_dope.md.strings > snd_hdr ->size_dope.md.strings)
	      ||
	      (rcv_hdr->size_dope.md.strings < snd_hdr -> snd_dope.md.strings)
	      )
	  {
	    r.msgdope |= L4_IPC_REMSGCUT;
	    tlb_flush_window(window_addr);
	    return r.msgdope;
	  }

	  /* set up pointers to recv and sender string dopes */
	  snd_strings = (l4_strdope_t *)
	    ((char *) snd_hdr + sizeof(l4_msghdr_t) +
	     8 * snd_hdr->size_dope.md.dwords);
	  rcv_strings = (l4_strdope_t *)
	    ((char *) rcv_hdr + sizeof(l4_msghdr_t) +
	     8 * rcv_hdr->size_dope.md.dwords);
	  /* save map addr for window onto dopes */
	  direct_map_addr = stcb->wdw_map_addr;
	  
	  for (i = 0; i < snd_hdr->snd_dope.md.strings; i++)
	  {
	    char *rp, *sp;
	    int j;

	    /* check string completely in user address space */
	    if ((snd_strings[i].snd_size > L4_MAX_STRING_SIZE) ||
		(rcv_strings[i].rcv_size > L4_MAX_STRING_SIZE) ||
		(snd_strings[i].snd_size > rcv_strings[i].rcv_size) ||
		((snd_strings[i].snd_str + snd_strings[i].snd_size) >=
		 USER_ADDR_TOP) ||
		((rcv_strings[i].rcv_str + rcv_strings[i].rcv_size) >=
		 USER_ADDR_TOP)
		)
	    {
	       r.msgdope |= L4_IPC_REMSGCUT;
	       tlb_flush_window(window_addr);
	       return r.msgdope;
	    }
	    /* okay to transfer string, determine pointers for copy */
	    sp = (char *)snd_strings[i].snd_str;
	    rp = (char *) (RECV_WINDOW_BASE +
	      (((stcb->myself & TID_THREAD_MASK) >> TID_THREAD_SHIFT)
	       * RECV_WINDOW_SIZE) +
	      ( rcv_strings[i].rcv_str & (4 * 1024 * 1024 - 1)));
	    j = (int) snd_strings[i].snd_size - 1;
	      
	    /* move window onto string */
	    stcb->wdw_map_addr =  rcv_strings[i].rcv_str &
	      (~(dword_t)(4*1024*1024 -1));
	    tlb_flush_window(window_addr);

	    /* FIXME: this is inefficient */
	    ints_on();
	    for (;j >= 0; j--)
	    {
	      rp[j] = sp[j];
	    }
	    ints_off();

	    /* move window back onto dopes */
	    stcb->wdw_map_addr =  direct_map_addr;
	    tlb_flush_window(window_addr);
	    
	    
	  }
	  r.md.strings = snd_hdr->snd_dope.md.strings;
	}
      }
      else
      {
	r.msgdope |= L4_IPC_REMSGCUT;
	return r.msgdope;
      }
    }
    else
    {
      r.msgdope |= L4_IPC_REMSGCUT;
    }
  }
  if (window_addr != 0)
  {
    tlb_flush_window(window_addr);
  }
  return r.msgdope;
}


