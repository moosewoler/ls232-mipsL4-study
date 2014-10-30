/****************************************************************************
 *      $Id: mem.c,v 1.1 1998/03/19 23:40:54 kevine Exp $
 *      Copyright (C) 1997, 1998 Kevin Elphinstone, University of New South
 *      Wales
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
#include "ipt.h"
#include <kernel/kmem.h>
#include <kernel/panic.h>
#include <kernel/trace.h>
#include <kernel/kernel.h>
#include <r4kc0.h>


void mt_mem_init(void)
{
  udw_t *p;
  udw_t f;
  kernel_vars *k;
  k = KERNEL_BASE;
  k->mt_free_list = 0;
  f = (udw_t) k_frame_alloc();
  if (f == 0)
  {
    panic("L4 PANIC: Could not initialise mapping tree memory pool"); /* ok */
  }
  
  for (p = (udw_t *) f; (udw_t) p <= f + FRAMESIZE - sizeof(mt_node_t);
       p +=  3)
  {
    *p = k->mt_free_list;
    k->mt_free_list = (udw_t) p;
  }
}

mt_node_t *mt_mem_alloc(void)
{
  udw_t *p;
  kernel_vars *k;
  k = KERNEL_BASE;

  if (k->mt_free_list != 0)
  {
    p = (udw_t *) k->mt_free_list;
    k->mt_free_list =  *p;
    return (mt_node_t *) p;
  }
  else
  {
    udw_t f;
    f = (udw_t) k_frame_alloc();
    if (f == 0)
    {
      panic("L4 PANIC: Mapping tree memory pool exhausted"); /* ok */
    }
    
    for (p = (udw_t *) f; (udw_t) p <= f + FRAMESIZE - sizeof(mt_node_t);
	 p +=  3)
    {
      *p = k->mt_free_list;
      k->mt_free_list = (udw_t) p;
    }
    p = (udw_t *) k->mt_free_list;
    k->mt_free_list =  *p;
    return (mt_node_t *) p;
  }
  
}

void mt_mem_free(mt_node_t  *p)
{
  kernel_vars *k;
  k = KERNEL_BASE;
  if (p == 0)
  {
    panic("L4 PANIC: mt_mem_free received a NULL");
  }
  *(udw_t *)p = k->mt_free_list;
  k->mt_free_list = (udw_t) p;
  
}



void ipt_node_init(void)
{
  udw_t *p;
  udw_t f;
  kernel_vars *k;
  k = KERNEL_BASE;
  k->gpt_free_list = 0;
  f = (udw_t) k_frame_alloc();
  if (f == 0)
  {
    panic("L4 PANIC: Could not initialise mapping tree memory pool"); /* ok */
  }
  
  for (p = (udw_t *) f; (udw_t) p <= f + FRAMESIZE - sizeof(ipt_t);
       p +=  4)
  {
    *p = k->gpt_free_list;
    k->gpt_free_list = (udw_t) p;
  }
}

ipt_t *ipt_node_alloc(void)
{
  udw_t *p;
  kernel_vars *k;
  k = KERNEL_BASE;

  if (k->gpt_free_list != 0)
  {
    p = (udw_t *) k->gpt_free_list;
    k->gpt_free_list =  *p;
    return (ipt_t *) p;
  }
  else
  {
    udw_t f;
    f = (udw_t) k_frame_alloc();
    if (f == 0)
    {
      panic("L4 PANIC: Mapping tree memory pool exhausted"); /* ok */
    }
    
    for (p = (udw_t *) f; (udw_t) p <= f + FRAMESIZE - sizeof(ipt_t);
	 p +=  4)
    {
      *p = k->gpt_free_list;
      k->gpt_free_list = (udw_t) p;
    }
    p = (udw_t *) k->gpt_free_list;
    k->gpt_free_list =  *p;
    return (ipt_t *) p;
  }
  
}

void ipt_node_free(ipt_t  *p)
{
  kernel_vars *k;
  k = KERNEL_BASE;
  if (p == 0)
  {
    panic("L4 PANIC: mt_mem_free received a NULL");
  }
  *(udw_t *)p = k->gpt_free_list;
  k->gpt_free_list = (udw_t) p;
}

#ifdef SIM
#include <stdio.h>
#include <stdlib.h>

ipt_t *ipt_head_alloc(void)
{
  return (ipt_t *) memalign(128 * 1024, 128 * 1024);
}
  
void ipt_head_free(ipt_t *p)
{
  free(p);
}

#else
udw_t *ipt_free_list_ptr;

ipt_t *ipt_head_alloc(void)
{
  udw_t *p;
  if (ipt_free_list_ptr != 0)
  {
    p = (udw_t *) ipt_free_list_ptr;
    ipt_free_list_ptr =  (udw_t *)*p;
    return (ipt_t *) p;
  }
  else
  {
    panic("L4 PANIC: Out of kernel memory for IPT");
  }
}

void ipt_head_free(ipt_t *p)
{
  if (p == 0)
  {
    panic("L4 PANIC: mt_mem_free received a NULL");
  }
  *(udw_t *)p = (udw_t)ipt_free_list_ptr;
  ipt_free_list_ptr = (udw_t*) p;
}

#endif
