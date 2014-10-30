/****************************************************************************
 *      $Id: sigma0.c,v 1.3 1998/01/22 05:49:51 kevine Exp $
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
#include <kernel/vm.h>
#include "panic.h"
#include "kmem.h"

#include <stdlib.h>
#include <stdio.h>

void sigma0_init(void *used_so_far, udw_t ram_size)
{
  printf("used_so_far = 0x%lx\n", used_so_far);
  printf("ram_size = 0x%lx\n", ram_size);
}


