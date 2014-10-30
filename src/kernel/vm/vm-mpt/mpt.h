#ifndef _MPT_H
#define _MPT_H
/****************************************************************************
 *      $Id: mpt.h,v 1.3 1998/03/19 23:41:03 kevine Exp $
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
#include <kernel/types.h>
#include <kernel/kernel.h>
#include <kernel/vm.h>

#define FRAMESIZE 4096
#define FRAME_SIZE_SHIFT 12

#define GPT_FPAGE_SIZE_MASK  0774
#define GPT_FPAGE_SIZE_SHIFT  2

#ifndef _LANGUAGE_ASSEMBLY
typedef w_t mpt_t;

typedef struct {
  uw_t   pte0;
  uw_t   pte1;
  w_t    mt_ptr0;
  w_t    mt_ptr1;
} mpt_leaf_t;

typedef struct _t_n {
  udw_t vaddr;
  w_t task;
  w_t child;
  w_t sister;
  w_t parent;
} mt_node_t;

/* internal mpt protos */
extern mpt_leaf_t *mpt_insert(tcb_t *tcb, udw_t);
extern void mpt_print(as_t *snd_gpt);
extern mpt_leaf_t *tcb_mpt_insert(tcb_t *tcb, udw_t vaddr);
extern void mpt_map(tcb_t *rcv_tcb, udw_t dst_addr, w_t *mt_ptr, uw_t *pte,
	     uw_t src_pte, w_t src_mt_ptr, uw_t read_write_mask);
extern void mpt_unmap(pte_t *pte, mt_node_t *start, udw_t read_write_mask,
		      tcb_t *tcb, dword_t mask);



/* internal memory allocators */
extern void mt_mem_init(void);
extern mt_node_t *mt_mem_alloc(void);
extern void mt_mem_free(mt_node_t  *p);

#endif

#endif
