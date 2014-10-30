#ifndef GPT_H
#define GPT_H
/****************************************************************************
 *      $Id: gpt.h,v 1.6 1998/01/22 05:48:32 kevine Exp $
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

#if 0

#define GPTROOTSIZE 1
#define GPTSIZE 1

#define GPTROOTSIZE 2
#define GPTSIZE 2

#define GPTROOTSIZE 1
#define GPTSIZE 3

#define GPTROOTSIZE 4
#define GPTSIZE 4

#define GPTROOTSIZE 2
#define GPTSIZE 5

#define GPTROOTSIZE 4
#define GPTSIZE 6

#define GPTROOTSIZE 3
#define GPTSIZE 7

#define GPTROOTSIZE 4
#define GPTSIZE 8

#endif

#define GPTROOTSIZE 4
#define GPTSIZE 8

#define FRAMESIZE 4096
#define FRAME_SIZE_SHIFT 12

#define GPT_FPAGE_SIZE_MASK  0774
#define GPT_FPAGE_SIZE_SHIFT  2


#ifndef _LANGUAGE_ASSEMBLY
#define WORDLEN 64ul
#else
#define WORDLEN 64
#endif


#ifndef _LANGUAGE_ASSEMBLY

/* gpt node defs */

typedef struct {
  uw_t   pte;
  w_t    mt_ptr;
} gpt_leaf_t;

typedef union {
  udw_t ptr;
  gpt_leaf_t lf;
} gpt_u_t;

typedef struct {
  udw_t guard;
  gpt_u_t u;
} gpt_t;

/* mapping tree node defs */

typedef struct _t_n {
  udw_t vaddr;
  w_t task;
  w_t child;
  w_t sister;
  w_t parent;
} mt_node_t;

/* internal protos */

/* Define a NULL_GUARD to signify an empty entry and cause a mismatch on
 * lookup.
 */

#define NULL_GUARD(level,size,u) ((~u & ((1 << size) -1)) << (level - size))



/* external protos */
extern gpt_t *gpt_init(void);
extern gpt_leaf_t * gpt_insert(tcb_t *tcb, udw_t);

/* memory allocators */
extern void gpt_leaf_mem_init(void);
extern gpt_leaf_t *gpt_leaf_alloc(void);
extern void gpt_leaf_free(gpt_leaf_t *p);

extern void gpt_mem_init(void);
extern gpt_t *gpt_mem_alloc(void);
extern void gpt_mem_free(gpt_t *p);

extern void mt_mem_init(void);
extern mt_node_t *mt_mem_alloc(void);
extern void mt_mem_free(mt_node_t  *p);




extern dw_t gpt_lookup(gpt_t *gpt, 
			 udw_t vaddr, udw_t *size, udw_t *paddr);
extern gpt_leaf_t *gpt_lookup_ptr(gpt_t *, udw_t vaddr);



extern dw_t gpt_remove(gpt_t *gpt, udw_t vaddr);

extern void gpt_check(gpt_t *gpt, 
	       udw_t gpt_size, 
	       udw_t level, 
	       udw_t *leaf);

extern void gpt_map(gpt_t *snd_gpt, udw_t snd_fpage, udw_t snd_cp,
	     gpt_t *rec_gpt, udw_t rec_fpage, w_t task);

extern void gpt_fpage_unmap(udw_t fpage, dw_t mask, gpt_t *gpt);

extern void tlb2_sync(udw_t vaddr, uw_t pte, w_t short_task_id);
extern void tlb2_sync_shared(udw_t vaddr, uw_t pte);
extern void task_tcb_unmap(udw_t vaddr,gpt_t *gpt);

extern void gpt_print(gpt_t *snd_gpt);

extern void gpt_mem_init(void);
extern gpt_t *gpt_mem_alloc(void);
extern void gpt_mem_free(gpt_t *p);
extern void mt_mem_init(void);
extern mt_node_t *mt_mem_alloc(void);
extern void mt_mem_free(mt_node_t  *p);

extern void pop_pos(gpt_t **, unsigned short *, unsigned char *,
		    unsigned char *,void **);
extern void push_pos(gpt_t *, unsigned short,  unsigned char,
		     unsigned char, void **);

#endif
#endif /* GPT_H */



