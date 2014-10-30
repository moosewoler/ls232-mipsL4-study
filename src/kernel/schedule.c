/****************************************************************************
 *      $Id: schedule.c,v 1.20 1999/09/09 03:17:31 gernot Exp $
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
#include <kernel/panic.h>
#include <kernel/trace.h>
#include <stdio.h>

#define KDEBUG

void wakeup_to_busy(kernel_vars *k, tcb_t *w)
{
  if (w->busy_link == 0) 
  {
    if (k->prio_busy_list[w->tsp] != 0)
    {
      /* old queue was already non-empty */
      w->busy_link = k->prio_busy_list[w->tsp]->busy_link;
      k->prio_busy_list[w->tsp]->busy_link = w;
    }
    else /* old queue was empty */
    {
      w->busy_link = w;
      k->prio_busy_list[w->tsp] = w;
    }

    /* wakeup thread has no time left - put at end of queue */
    if (k->timeslice == 0)
    {
      k->prio_busy_list[w->tsp] = w;
      w->rem_timeslice = w->timeslice;
      w->ctsp = w->tsp;
    }
  }
}


tcb_t * get_next_thread(kernel_vars *k)
{
  tcb_t *t1,*t2;
  short int i;
  char c[100];

  /* check interrupted stack first */
  if (k->int_list != END_LIST)
  {
    t2 = k->int_list;
    k->int_list = t2->int_link;
    return t2;
  }

  for(i = MAX_PRIORITY; i >= 0; i--)
  {
    t1 = k->prio_busy_list[i];

    /* list at priority i is empty; check next priority */
    if (t1 == 0)
      continue;

    t2 = t1->busy_link;

    while(
	  (t2 != (tcb_t *)0) &&
	  ((t2->fine_state & FS_BUSY) == 0)
	  )
   {
     /* remove non busy entries from busy_list */
     t1->busy_link = t2->busy_link;
     t2->busy_link = 0;
     t2 = t1->busy_link; 
    }

    /* all entries at this priority were removed; check next priority */
    if (t2 == 0)
    {
      k->prio_busy_list[i] = 0;
      continue;
    }
    break;
  }

#ifdef KDEBUG
    /* all lists are empty */
    if (t2 == 0)
    {
      panic("L4 PANIC: busy list corrupt"); /* ok */
    }

    /* tcb prioriy and list priority do no match */
    if (t2->tsp != i) 
    {
      sprintf(c, "L4 PANIC: busy list corrupt; entry (tsp = %d) is in wrong queue (prio = %d)", t2->tsp, i);
      panic(c);	/* ok */
    }
#endif

  k->prio_busy_list[i] = t2;
  return t2;
}

extern void pending_wakeup();
extern void receive_wakeup();

tcb_t *check_wake_up(void)
{
  tcb_t *p, *prev, *r;
  tcb_t *hp_wakeup = 0; /* highest priority wakeup thread */
  unsigned long compare_time;
  kernel_vars *k;

#ifdef NOSGI
  char c[100];
#endif

  k = KERNEL_BASE;
  r = 0;
  
  if ((k->clock & (LATE_TIME - 1)) == 0)
  {
    /* process late wakeup queue */
    prev = 0;
    p = k->late_wakeup_list;
    compare_time = k->clock + LATE_TIME;
    while (p != END_LIST)
    {
      if ((p->fine_state & FS_WAKEUP) == 0)
      {
	/* remove from list */
	if (prev == 0)
	{
	  k->late_wakeup_list = p->late_wakeup_link;
	  p->late_wakeup_link = OUT_LIST;
	  p = k->late_wakeup_list;
	}
	else
	{
	  prev->late_wakeup_link = p->late_wakeup_link;
	  p->late_wakeup_link = OUT_LIST;
	  p = prev->late_wakeup_link;
	}
	continue;
      }
      
      if (p->wakeup < compare_time)
      {
	tcb_t *t;
	/* move to next shorter term wakeup list  */
	t = p;
	/* remove from list */
	if (prev == 0)
	{
	  k->late_wakeup_list = p->late_wakeup_link;
	  p = k->late_wakeup_list;
	}
	else
	{
	  prev->late_wakeup_link = p->late_wakeup_link;
	  p = prev->late_wakeup_link;
	}
	t->late_wakeup_link = OUT_LIST;
	
	/* add to shorter term list */
	if (t->wakeup_link == OUT_LIST)
	{
	  t->wakeup_link = k->wakeup_list;
	  k->wakeup_list = t;
	}
	continue;
      }
      prev = p;
      p = p->late_wakeup_link;
    }
  }

  if ((k->clock & (SOON_TIME - 1)) == 0)
  {
    /* process medium term list */
    prev = 0;
    p = k->wakeup_list;
    compare_time = k->clock + SOON_TIME;
    while (p != END_LIST)
    {
      if ((p->fine_state & FS_WAKEUP) == 0)
      {
	/* remove from list */
	if (prev == 0)
	{
	  k->wakeup_list = p->wakeup_link;
	  p->wakeup_link = OUT_LIST;
	  p = k->wakeup_list;
	}
	else
	{
	  prev->wakeup_link = p->wakeup_link;
	  p->wakeup_link = OUT_LIST;
	  p = prev->wakeup_link;
	}
	continue;
      }
      
      if (p->wakeup < compare_time)
      {
	tcb_t *t;
	/* move to next shorter term wakeup list  */
	t = p;
	/* remove from list */
	if (prev == 0)
	{
	  k->wakeup_list = p->wakeup_link;
	  p = k->wakeup_list;
	}
	else
	{
	  prev->wakeup_link = p->wakeup_link;
	  p = prev->wakeup_link;
	}
	t->wakeup_link = OUT_LIST;
	
	/* add to shorter term list */
	if (t->soon_wakeup_link == OUT_LIST)
	{
	  t->soon_wakeup_link = k->soon_wakeup_list;
	  k->soon_wakeup_list = t;
	}
	continue;
      }
      prev = p;
      p = p->wakeup_link;
    }

  }

  /* process short term list */
  
  prev = 0;
  p = k->soon_wakeup_list;
  compare_time = k->clock;
  while (p != END_LIST)
  {
    tcb_t *tmp;

    if ((p->fine_state & FS_WAKEUP) == 0)
    {
      /* remove from list */
      if (prev == 0)
      {
	k->soon_wakeup_list = p->soon_wakeup_link;
	p->soon_wakeup_link = OUT_LIST;
	p = k->soon_wakeup_list;
      }
      else
      {
	prev->soon_wakeup_link = p->soon_wakeup_link;
	p->soon_wakeup_link = OUT_LIST;
	p = prev->soon_wakeup_link;
      }
      continue;
    }
    if (p->wakeup <= compare_time)
    {
      /* okay have to wake up task */
      r = p;
      /* remove from wakeup list */
      if (prev == 0)
      {
	k->soon_wakeup_list = p->soon_wakeup_link;
	p->soon_wakeup_link = OUT_LIST;
	p = k->soon_wakeup_list;
      }
      else
      {
	prev->soon_wakeup_link = p->soon_wakeup_link;
	p->soon_wakeup_link = OUT_LIST;
	p = prev->soon_wakeup_link;
      }
      if (r->fine_state & FS_POLL)
      {
	*(unsigned long *)(r->stack_pointer) = (unsigned long) pending_wakeup;
      }

      else if (r->fine_state & FS_WAIT)
      {
	r->stack_pointer -= 8ul;
	*(unsigned long *)(r->stack_pointer) = (unsigned long) receive_wakeup;
      }
      
      if (hp_wakeup == 0)
	hp_wakeup = r;
      else /* mark wakened thread BUSY */
      {
	/* update new highest priority wakeup thread if necessary */
	if (hp_wakeup->ctsp < r->ctsp)
	{
	  tmp = hp_wakeup;
	  hp_wakeup = r;
	  r = tmp;
	}
	r->fine_state = FS_BUSY;
	wakeup_to_busy(k, r);
      }
      continue;
    }
    prev = p;
    p = p->soon_wakeup_link;
  }
  
  if (hp_wakeup != 0)
  {
    if (hp_wakeup->ctsp <= k->priority || k->timeslice == 0)
    {
      /* highest priority wakeup thread has lower priority than current 
       running thread or has no time left*/
      hp_wakeup->fine_state = FS_BUSY;
      wakeup_to_busy(k, hp_wakeup);
      r = 0;
    }
    else 
      return (hp_wakeup);
  }


  
  if (r == 0 && (k->timeslice == 0))
  {
#if 0 && defined(NOSGI)
      dbg();
#endif
    /* preempt current task */
    r = get_next_thread(k);
#if 0 &&  defined(NOSGI)
     sprintf(c, "new thread is 0x%llx\n", r);
      panic(c);
      dbg();
#endif

    
  }
  return r;
}

void process_lists(void)
{
  kernel_vars *k;
  tcb_t *p, *n;
  short int i;
  
  k = KERNEL_BASE;

  /* remove non-busy tcbs from busy lists */
  for (i = 0; i <= MAX_PRIORITY; i++)
  {
    /* check if this queue is empty */
    if (k->prio_busy_list[i] == 0)
      continue;

    p = k->prio_busy_list[i];
    n = p->busy_link;

    while (n != k->prio_busy_list[i]) {
      if (n->fine_state & FS_BUSY)
      {
	p = n;
	n = p->busy_link;
      }
      else
      {
	p->busy_link = n->busy_link;
	n->busy_link = 0;
	n = p->busy_link;
      }
    } 
    if ((n->fine_state & FS_BUSY) == 0)
    {
      if (n == p) /* last tcb in list is non-busy */
	k->prio_busy_list[i] = 0;
      else
      {
	  p->busy_link = n->busy_link;
	  k->prio_busy_list[i] = p;
      }
      n->busy_link = 0;
    }
  }

  /* remove non-waking tcbs from wakeup lists */
  p = 0;
  n = k->late_wakeup_list;

  while (n != END_LIST)
  {
    if ((n->fine_state & FS_WAKEUP) == 0)
    {
      /* remove from list */
      if (p == 0)
      {
	k->late_wakeup_list = n->late_wakeup_link;
	n->late_wakeup_link = OUT_LIST;
	n = k->late_wakeup_list;
      }
      else
      {
	p->late_wakeup_link = n->late_wakeup_link;
	n->late_wakeup_link = OUT_LIST;
	n = p->late_wakeup_link;
      }
    }
    else
    {
      p = n;
      n = p->late_wakeup_link;
    }
  }

  p = 0;
  n = k->wakeup_list;

  while (n != END_LIST)
  {
    if ((n->fine_state & FS_WAKEUP) == 0)
    {
      /* remove from list */
      if (p == 0)
      {
	k->wakeup_list = n->wakeup_link;
	n->wakeup_link = OUT_LIST;
	n = k->wakeup_list;
      }
      else
      {
	p->wakeup_link = n->wakeup_link;
	n->wakeup_link = OUT_LIST;
	n = p->wakeup_link;
      }
    }
    else
    {
      p = n;
      n = p->wakeup_link;
    }
      
  }

  p = 0;
  n = k->soon_wakeup_list;

  while (n != END_LIST)
  {
    if ((n->fine_state & FS_WAKEUP) == 0)
    {
      /* remove from list */
      if (p == 0)
      {
	k->soon_wakeup_list = n->soon_wakeup_link;
	n->soon_wakeup_link = OUT_LIST;
	n = k->soon_wakeup_list;
      }
      else
      {
	p->soon_wakeup_link = n->soon_wakeup_link;
	n->soon_wakeup_link = OUT_LIST;
	n = p->soon_wakeup_link;
      }
    }
    else
    {
      p = n;
      n = p->soon_wakeup_link;
    }
  }
  return;
}

    
