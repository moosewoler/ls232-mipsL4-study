#ifndef KMEM_H
#define KMEM_H
/****************************************************************************
 * $Id: kmem.h,v 1.8 1998/01/22 05:46:08 kevine Exp $
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

#include "types.h"

extern void dcache_hit_wb(void *);
extern void dcache_page_invalidate(void *);

/* kernel frame management routines */
extern void k_frame_free(void *p);
extern void *k_frame_alloc(void);
extern void *tcb_frame_alloc(void);
extern void tcb_frame_free(void *v, void *p);
     
/* routines to manage frames in physical memory */
extern void *frame_alloc(void);
extern void *sigma0_frame_alloc(void);
extern void frame_free(void *);


/* old routines to manage 32 byte blocks */
/* extern void kmem_blocks_init(void);
extern void *kmem_32_alloc(void);
extern void kmem_32_free(void *p); */

#endif
