#ifndef GPT_H
#define GPT_H
/****************************************************************************
 *      $Id: ipt.h,v 1.1 1998/03/19 23:40:29 kevine Exp $
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

#define CPT_NODE_SIZE 128
#define CPT_OVERFLOW_NODE_SIZE    80
#define CPT_POOL_SIZE 7
#define TCB_IPT_ENTRIES 4096
#define TCB_ARRAY_SIZE_MASK  0xffful

#ifdef CPT_128

#define CPT_INITIAL_SIZE    (128 * 1024)
#define CPT_ENTRIES  (CPT_INITIAL_SIZE / CPT_NODE_SIZE)
#define CPT_COVERAGE (4096 * CPT_ENTRIES * 8)
#define CPT_ARRAY_SIZE_MASK 0x3fful

#elif defined(CPT_8)

#define CPT_INITIAL_SIZE    (8 * 1024)
#define CPT_ENTRIES  (CPT_INITIAL_SIZE / CPT_NODE_SIZE)
#define CPT_COVERAGE (4096 * CPT_ENTRIES * 2)
#define CPT_ARRAY_SIZE_MASK 0x3ful

#else

#error no ipt size specified

#endif

#define IPT_NODE_SIZE 32
#define IPT_POOL_SIZE 5
#define TCB_IPT_ENTRIES 4096

#ifndef _LANGUAGE_ASSEMBLY
#define WORDLEN 64ul
#else
#define WORDLEN 64
#endif


#ifndef _LANGUAGE_ASSEMBLY

/* ipt node defs */

typedef struct {
  udw_t  tag;
  uw_t   pte0;
  uw_t   pte1;
  udw_t  link;
  w_t    mt_ptr0;
  w_t    mt_ptr1;
} ipt_t;

typedef struct {
  udw_t  tag;
  uw_t   pte[8];
  udw_t  link;
  w_t    mt_ptr[8];
} cpt_t;

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


/* external protos */
extern cpt_t * cpt_insert(tcb_t *tcb, udw_t);
extern ipt_t * tcb_ipt_insert(udw_t);
extern void cpt_remove(cpt_t *ipt, udw_t vaddr);
extern ipt_t *ipt_lookup_pair(ipt_t *, udw_t vaddr);
extern cpt_t *cpt_lookup_bucket(cpt_t *, udw_t vaddr);

/* memory allocators */

extern void mt_mem_init(void);
extern mt_node_t *mt_mem_alloc(void);
extern void mt_mem_free(mt_node_t  *p);

extern void ipt_node_init(void);
extern ipt_t *ipt_node_alloc(void);
extern void ipt_node_free(ipt_t *);

extern void cpt_node_init(void);
extern cpt_t *cpt_node_alloc(void);
extern void cpt_node_free(cpt_t *);

extern void cpt_head_init(void);
extern void cpt_unmap(pte_t *pte, mt_node_t *mt_ptr,
		      udw_t read_write_mask, tcb_t *tcb, dword_t mask);


extern void tcb_ipt_init(void);
extern cpt_t *cpt_init(void);
extern void cpt_free(cpt_t *);
extern cpt_t *cpt_head_alloc(void);
extern void cpt_head_free(cpt_t *);

extern void cpt_map(tcb_t *rcv_tcb, udw_t dst_addr, w_t *mt_ptr, uw_t *pte,
	       uw_t src_pte, w_t src_mt_ptr, uw_t read_write_mask);
#ifdef SIM
extern dword_t  *tlb2_base_ptr;
#endif

#endif
#endif /* GPT_H */



