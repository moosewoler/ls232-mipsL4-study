/****************************************************************************
 *      $Id: kmem.c,v 1.4 1998/01/22 05:49:45 kevine Exp $
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

#include <stdio.h>
#include <stdlib.h>

#include <kernel/kmem.h>
#include <kernel/panic.h>

#include <r4kc0.h>
#include "sim.h"

void *k_frame_alloc(void)
{
  udw_t *p,*z;;


  
  p = memalign(FRAMESIZE,FRAMESIZE);
  for(z = p; z < p + (FRAMESIZE/sizeof(*p));z++)
  {
    *z = ~EL_Valid;
  }
	
  if (p == 0)
  {
    panic("L4 PANIC: Reserved kernel memory exhausted"); /* ok */
  }
  return (void *) p;
}

void k_frame_free(void *p)
{

  if (p == 0)
  {
    panic("freeing null pointer");
  }
  free(p);
}

void *tcb_frame_alloc(void)
{
  udw_t *p;

  p = memalign(FRAMESIZE,FRAMESIZE);
  if (p == 0)
  {
    panic("L4 PANIC: Reserved kernel memory exhausted"); /* ok */
  }
  return (void *) p;
}

void tcb_frame_free(void *v, void *p)
{
   if (p == 0)
  {
    panic("freeing null pointer");
  }
  free(p);
}
