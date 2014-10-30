/****************************************************************************
 * $Id: tt3.c,v 1.4 1998/12/28 12:23:21 gernot Exp $
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
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>

static const l4_threadid_t upperpagerid = {0x200a000000220401};

void main(void)
{
  l4_threadid_t tid, t1, t2, page, pre;

  tid = l4_myself();
  pre.ID = -1;
  page.ID = upperpagerid.ID;
  
  t1.id.task = tid.id.task + 12;
  t2.id.task = tid.id.task + 13;
  t1 = l4_task_new(t1, 0,  
		     0x0009fff8,
		     0x000a0118,
		     upperpagerid, l4_myself());

  t2 = l4_task_new(t2, 0,  
		     0x000afff8,
		     0x000b0118,
		     upperpagerid, l4_myself());

  while(1);
}


