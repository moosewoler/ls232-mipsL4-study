/****************************************************************************
 *      $Id: kmem.c,v 1.15 1999/09/09 03:17:30 gernot Exp $
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

#include <kernel/kmem.h>
#include <kernel/panic.h>
#include <kernel/trace.h>
#include <kernel/kernel.h>
#include <r4kc0.h>


void *k_frame_alloc(void)
{
  udw_t p;
  kernel_vars *k;

  
  k = KERNEL_BASE;
  if (k->frame_list == 0)
  {
    panic("L4 PANIC: Reserved kernel memory exhausted"); /* FIXME!!! */
  }
  p = k->frame_list;
  k->frame_list = *(udw_t *) p;
#ifdef KDEBUG  
  if (CKSEG0_TO_PHYS((udw_t)p + INDY_OFFSET) > k->memory_size)
  {
    panic("L4 PANIC: frame free list corrupt");	/* ok */
  }
#endif  
  return (void *) p;
}

void k_frame_free(void *p)
{

  /* assume who ever had page has flushed dirty cache lines to ensure a
     clean page */
  kernel_vars *k;
  k = KERNEL_BASE;
  if (p == 0)
  {
    panic("L4 PANIC: k_frame_free received a NULL");	/* ok */
  }
  
  *(udw_t *)p = k->frame_list;
  dcache_hit_wb(p);
  k->frame_list = (udw_t) p;
}

void *tcb_frame_alloc(void)
{
  return (void *) CKSEG0_TO_PHYS((udw_t) k_frame_alloc()) ;
}

void tcb_frame_free(void *v, void *p)
{
  /* flush v out of cache to avoid free list corruption */
  dcache_page_invalidate(v);
  
  p = (void *)PHYS_TO_CKSEG0((udw_t) p);
  k_frame_free(p);
}

