#ifndef SIM_H
#define SIM_H
/****************************************************************************
 *      $Id: sim.h,v 1.4 1998/01/22 05:49:51 kevine Exp $
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

#include <l4/types.h>
#include <kernel/types.h>

#define FRAMESIZE 4096
#define FRAME_SIZE_SHIFT 12

#define MAXAS 32
#define NILAS (MAXAS + 1)
#define PAGER_DEPTH 5
#define FAN_OUT 3
#define MEGRAM 64
#define RESERVED_RAM 1
#define MAXPG (MEGRAM * 256)

#include <kernel/kernel.h>

#if defined(_LANGUAGE_C)

typedef struct sim_as {
  tcb_t tcb;
  word_t pager;
  byte_t v_table[MAXPG];
  word_t child[FAN_OUT];
} sim_as_t;



extern void sigma0_init(void *used_so_far, udw_t ram_size);

extern dword_t c0_ehi;
extern dword_t c0_bva;
extern word_t c0_count;
extern sim_as_t vas[];
#endif

#endif
