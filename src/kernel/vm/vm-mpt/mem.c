/****************************************************************************
 *      $Id: mem.c,v 1.2 1998/01/22 05:48:52 kevine Exp $
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
#include "mpt.h"
#include <kernel/kmem.h>
#include <kernel/panic.h>
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
#ifdef KDEBUG  
  if (p == 0)
  {
    panic("L4 PANIC: mt_mem_free received a NULL");
  }
#endif  
  *(udw_t *)p = k->mt_free_list;
  k->mt_free_list = (udw_t) p;
  
}
