/****************************************************************************
 *      $Id: gpt.c,v 1.4 1999/09/11 05:55:08 gernot Exp $
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

#include <r4kc0.h>
#include "gptdefs.h"
#include <kernel/panic.h>
#include <kernel/kmem.h>
#include <kernel/trace.h>
#include <kernel/kutils.h>
#include <kernel/kernel.h>
#include <l4/types.h>
#include <l4/sigma0.h>
#include <alloca.h>
#ifdef SIM
#include <sim.h>
#endif
extern void pop_pos(gpt_t **, unsigned char *, unsigned char *, void **);
extern void push_pos(gpt_t *, unsigned char,  unsigned char, void **);

/****************************************************************************
 *
 * gpt_init: Create and initialise the top node of the GPT tree
 *
 ****************************************************************************/

gpt_t *gpt_init(void)
{
  LABEL("gpti");
  gpt_t *gpt;
  dw_t i;
  /* TRACE(T_PROCS); */
    

  if ( gpt = gpt_mem_alloc() )
  {
    for (i = 0; i < ( 1ul << GPTROOTSIZE); i++)
    {
      gpt[i].guard = NULL_GUARD(WORDLEN,GPTROOTSIZE,i);
      gpt[i].u.ptr = 0;
    }
  }
  return gpt;
}

/****************************************************************************
 *
 * gpt_insert: Inserts a new entry into a given tree. Will fail and return
 * -1 if an overlapping entry exits.
 *
 * Input variables:
 * 	gpt: 		A pointer to the top level in tree.
 * 	gpt_size:	Size (log 2) of top level in tree.
 * 	vaddr:		virtual address to insert (page aligned).
 * 	pg_size:	size of page to insert.
 * 	paddr		physical address associated with virtual address.
 *
 ****************************************************************************/

gpt_leaf_t *gpt_insert(gpt_t *gpt, 
		       udw_t vaddr, gpt_leaf_t leaf)
{
  udw_t level, u, s;
  gpt_t *ptr;
  
  level = WORDLEN;
  s = GPTROOTSIZE;
  ptr = gpt;
  
  do {
    /* get u */
    u = (vaddr >> (level - s)) & ((1ul << s) - 1ul);
    
    if (ptr[u].guard == NULL_GUARD(level,s,u))
    {
      /* insert here */
      udw_t s0, s1, glen, ubar, g;
      
      s1 = L4_LOG2_PAGESIZE;
      glen = level - s - L4_LOG2_PAGESIZE;
      
      if (glen == 0)
      { /* encode zero length leaf guards differently */
	s0 = WORDLEN - s;
	ubar = (1 << s) + u;
      }
      else
      {
	s0 = WORDLEN - glen;
	ubar = ~u & ((1ul << s) -1ul);
      }
      g = (vaddr >> L4_LOG2_PAGESIZE) & ((1ul << glen) - 1ul);
      
      ptr[u].guard = 
	(((ubar << glen) + g) <<  L4_LOG2_PAGESIZE ) + (s0 << 6ul) + s1;
      
      ptr[u].u.lf = leaf;
      return &ptr[u].u.lf;
    }
    else
    {
      udw_t s1, glen;
      
      /* check for guard match */
      s1 = ptr[u].guard & 077ul;
      glen = level -s - s1;
      if (
	  (vaddr >> s1 & ((1ul << glen) - 1ul)) ==
	  (ptr[u].guard >> s1 & ((1ul << glen) - 1ul))
	  )
      {
	/* guard match so follow link */
	
	/* check if too low in tree to add page size */
	if ( s1 <=  L4_LOG2_PAGESIZE)
	{
	  /* this can happen when inserting a larger page over
	     a smaller one */
	  return 0;
	}
	
	/* check if leaf and can't go further */
	if (
	    (~u & ((1ul << s) - 1ul)) == 
	    ((ptr[u].guard >> (level - s)) & ((1ul << s) - 1ul))
	    ||
	    (((1ul << s) + u) ==
	     ((ptr[u].guard >> (level - s)) & ((1ul << (s+1)) -1ul))
	     )
	    )
	  
	{
	  /* trying to insert new entry that overlaps with 
	   * one already in the tree
	   */
	  return 0;
	}
	level = s1;
	s = s1 - (ptr[u].guard >> 6ul & 077ul) - 4ul;
	ptr = (gpt_t *)(ptr[u].u.ptr - 15ul);
      }
      else
      {
	/* guard mismatch, we need to 
	 * split guard and add new node
	 */
	gpt_t *new_node;
	udw_t g, v, nu, ng, ns0, ns1;;
	
	/* see how much of the guard matches */
	g = ptr[u].guard >> s1 & ((1ul << glen) - 1ul);
	v = vaddr >> s1 & ((1ul << glen) - 1ul);
	
	while (glen > 0)
	{
	  if (g != v)
	  {
	    g = g >> GPTSIZE;
	    v = v >> GPTSIZE;
	    glen -= GPTSIZE;
	  }
	  else
	  {
	    break;
	  }
	}
	
	if (level -s -glen <= L4_LOG2_PAGESIZE)
	{
	  /* attempting a split that will lead to a clash
	     anyway */
	  return 0; }
	
	/* allocate a new node */
	if ( (new_node = gpt_mem_alloc()) )
	{
	  udw_t i;
	  for (i = 0; i < 1ul << GPTSIZE; i++)
	  {
	    new_node[i].guard =
	      NULL_GUARD(level -s -glen,GPTSIZE,i);
	    new_node[i].u.ptr = 0;
	  }
	}
	else
	{
	  panic("L4 PANIC: page table memory alloc returned zero"); /* ok */
	}
	
	/* determine parts of guard for lower half of split
	 */
	
	nu = (ptr[u].guard  >> (level - s - glen - GPTSIZE)) &
	  ((1ul << GPTSIZE) - 1ul);
	ng = ptr[u].guard >> s1 &
	  ((1ul << (level - s - glen - GPTSIZE - s1)) - 1ul);
	
	/* check if leaf
	 */
	if (
	    (~u & ((1ul << s) -1ul)) ==
	    (ptr[u].guard >> (level - s) & ((1ul << s) -1ul))
	    ||
	    (((1ul << s) + u) ==
	     ((ptr[u].guard >> (level - s)) & ((1ul << (s+1)) -1ul))
	     )
	    
	    )
	{
	  udw_t tmpu;
	  /* lower node is a leaf
	   */
	  if ((level-s-glen-GPTSIZE-s1) == 0)
	  {
	    int index;

	    tmpu = (vaddr >> L4_LOG2_PAGESIZE) & ((1ul << GPTSIZE) - 1ul);
	    
	    if (( (long) vaddr) >= 0)
	    {
	      ns0 = 0;
	    }
	    else
	    {
	      ns0 = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
		EL_Cache_CNWb |  EL_Valid | EL_Global;
	    }
	    
	    for (index = 0; index < (1 << GPTSIZE); index ++)
	    {
	      if (index != tmpu)
	      {
		new_node[index].guard = (((1ul << GPTSIZE) + index)
					 << L4_LOG2_PAGESIZE) +
		  ((WORDLEN - GPTSIZE) << 6) +
		  L4_LOG2_PAGESIZE;
		new_node[index].u.lf.pte = ns0;
	      }
	    }
	    tmpu = (1ul << GPTSIZE) + nu;
	    ns0 = WORDLEN - GPTSIZE;

	  }
	  else
	  {
	    tmpu = (~nu & ((1ul << GPTSIZE) - 1ul));
	    ns0 = (WORDLEN - (level-s-glen-GPTSIZE-s1));
	  }
	  ns1 = s1;
	  
	  /* fix lower node */
	  new_node[nu].guard =
	    (((tmpu
	       << (level-s-glen-GPTSIZE-s1)) + ng) << s1)
	    + (ns0 << 6ul) + s1;
	}
	else
	{
	  /* lower node not a leaf */
	  new_node[nu].guard =
	    ((nu << (level - s - glen - GPTSIZE - s1)) + ng)
		 << s1 | (ptr[u].guard & 07777ul);
	}
	new_node[nu].u.ptr = ptr[u].u.ptr;
	
	/* fix old upper new node */
	ns1 = level - s - glen;
	ns0 = ns1 - GPTSIZE - 4;
	ng = ptr[u].guard >> ns1 & ((1ul << glen) -1ul);
	ptr[u].u.ptr = ((udw_t ) new_node) + 15ul;
	ptr[u].guard = (((u << glen) + ng) << ns1) +
	  (ns0 << 6ul) + ns1;
      }
    }
  } while (1);
}


/****************************************************************************
 *
 * gpt_remove: removes the entry associated with a virtual address. returns
 * -1 if entry not found.
 *
 * Input Variables:
 * 	gpt: pointer to top of tree.
 * 	gpt_size: size (log 2) of top of tree.
 * 	*vaddr: virtual address to remove.
 *
 * Output:
 * 	*vaddr: base of entry removed.
 * 	*size: page size of entry removed.
 * 	*paddr: physical address associated with entry removed.
 *
 ****************************************************************************/

dw_t gpt_remove(gpt_t *gpt, udw_t vaddr)
{
  udw_t u, s1, glen, level, s, ls, llevel, lu;
  gpt_t *ptr, *lptr;
  
  level = WORDLEN;
  s = GPTROOTSIZE;
  ptr = gpt;
  lptr = 0ul;
  do {
    u = (vaddr >> (level - s)) & ((1ul << s) - 1ul);
    
    /* if null entry return */
    if(ptr[u].guard == NULL_GUARD(level,s,u))
    {
      return -1l;
    }
    
    
    /* extract s1 and guard length */
    s1 = ptr[u].guard & 077ul;
    glen = level -s - s1;
    
    if (
	(vaddr >> s1 & ((1ul << glen) - 1ul)) ==
	(ptr[u].guard >> s1 & ((1ul << glen) - 1ul))
	)
    {
      /* guard match */
      /* check if leaf and can't go further */
      if (
	  (~u & ((1ul << s) - 1ul)) == 
	  ((ptr[u].guard >> (level - s)) & ((1ul << s) - 1ul))
	  ||
	  (((1ul << s) + u) ==
	   ((ptr[u].guard >> (level - s)) & ((1ul << (s+1ul)) -1ul))
	   )
	  )
      {
	udw_t i, count;
	/* have found leaf entry to remove */
	
	ptr[u].guard = NULL_GUARD(level,s,u);
	ptr[u].u.ptr = 0;
	
	/* now see if we can remove this node */
	
	if (lptr != 0ul)
	{
	  udw_t nu, ng, ns0, ns1;
	  count = 0;
	  for (i = 0; i < (1ul << s); i++)
	  {
	    if (ptr[i].guard != NULL_GUARD(level, s, i))
	    {
	      count++;
	      u = i;
	    }
	  }
	  if (count == 0ul)
	  {
	    /* only happens in case of non-optimal tree
	     */
	    panic("L4 PANIC: Page table not optimal (corrupt?)"); /* ok */
	  }
	  if (count > 1ul)
	  {
	    return 0ul;
	  }
	  
	  /* count == 1, need to merge this node with one above */
	  
	  ns1 = ptr[u].guard & 077ul;
	  ng = (((((lptr->guard >> level) &
		   ((1ul << (llevel - ls -level)) - 1ul)) << s) + u)
		<< (level -s -ns1))
	    + ((ptr[u].guard >> ns1 ) &
	       (( 1ul << (level -s -ns1)) -1ul));
	  if (
	      (~u & ((1ul << s) -1ul)) ==
	      (ptr[u].guard >> (level - s) & ((1ul << s) -1ul))
	      ||
	      (((1ul << s) + u) ==
	       ((ptr[u].guard >> (level - s)) &
		((1ul << (s+1ul)) -1ul)))
	      
	      )
	  {
	    /* we have a leaf */
	    nu = ~lu & ((1ul << ls) - 1ul);
	    ns0 = WORDLEN - (llevel - ls - ns1);
	  }
	  else
	  {
	    /* non leaf */
	    nu = lu;
	    ns0 = (ptr[u].guard >> 6ul) & 077ul;
	  }
	  lptr->u.ptr = ptr[u].u.ptr;
	  lptr->guard = (((nu << (llevel - ls -ns1)) + ng) << ns1)
	    + (ns0 << 6ul) + ns1;
	  
	  gpt_mem_free(ptr);
	}
	return 0;
      }
      llevel = level;
      level = s1;
      ls = s;
      s = s1 - ((ptr[u].guard >> 6ul) & 077ul) - 4ul;
      lptr = &ptr[u];
      ptr = (gpt_t *)(ptr[u].u.ptr - 15ul);
      lu = u;
      
    }
    else
    {
      /* guard mismatch */
      return -1l;
    }
  } while (1);
}

/****************************************************************************
 *
 * gpt_map: performs a map operation between two address spaces. Always
 * succeeds even though it may do nothing. Also updates mapping trees.
 *
 * Inputs:
 * 	snd_gpt: pointer to sender gpt
 * 	rec_gpt: pointer to receiver gpt
 * 	snd_fpage:
 * 	rec_fpage: fpage descriptors
 * 	snd_cp: sender center point of fpage
 *
 ****************************************************************************/

void gpt_map(gpt_t *snd_gpt, udw_t snd_fpage, udw_t snd_cp,
	     gpt_t *rec_gpt, udw_t rec_fpage, w_t rec_task_id)
{
  udw_t snd_size_mask;
  udw_t rec_size_mask;
  udw_t snd_base, rec_base, map_size;
  uw_t read_write_mask;
  gpt_leaf_t *src, *dst;

  if ((snd_fpage & GPT_FPAGE_SIZE_MASK) == 256)
  {
    snd_fpage -= 4;	/* size=63, avoids overflow */
  }
  snd_size_mask = (1ul << ((snd_fpage & GPT_FPAGE_SIZE_MASK)
			 >> GPT_FPAGE_SIZE_SHIFT)) - 1;
  
  if ((rec_fpage & GPT_FPAGE_SIZE_MASK) == 256)
  {
    rec_size_mask = ~0ul;
  }
  else
  {
    rec_size_mask = (1ul << ((rec_fpage & GPT_FPAGE_SIZE_MASK)
			   >> GPT_FPAGE_SIZE_SHIFT)) - 1;
  }
  
  snd_base = snd_fpage & ~snd_size_mask |
    snd_cp & snd_size_mask & ~rec_size_mask;
  
  rec_base = rec_fpage & ~rec_size_mask |
    snd_cp & rec_size_mask & ~snd_size_mask;
  
  map_size = (snd_size_mask & rec_size_mask) + 1;

  if (map_size != FRAMESIZE)
  {
    panic("L4 PANIC: Tried to send fpage > 4K, not currently supported "); /* ok */
  }

  if (snd_fpage & L4_FPAGE_GRANT_MASK)
  {
    panic("L4 PANIC: Tried to grant fpage, not currently supported"); /* ok */
  }

  if (snd_fpage & L4_FPAGE_RW_MASK)
  {
    read_write_mask = ~ 0;
  }
  else
  {
    read_write_mask = ~  EL_Dirty;
  }

  src  = gpt_lookup_ptr(snd_gpt, snd_base);
  
  if (src == 0)
  {
    /* sending nonexistent page -> nop */
    return;
  }
  else if ((src->pte & EL_Valid) == 0)
  {
    /* sending invalid page -> nop */
    return;
  }
  else
  {
    /* sending valid -> do something */
    dst =  gpt_lookup_ptr(rec_gpt, rec_base);
    if (dst == 0)
    {
      gpt_leaf_t leaf;
      mt_node_t *tree_ptr;
      
      /* add new node to the the mapping tree */

      tree_ptr = (mt_node_t *) mt_mem_alloc();


      /* receiver gpt invalid (empty) -> copy in new value */
      leaf.mt_ptr = (w_t) tree_ptr;
      leaf.pte = src->pte & read_write_mask;
      
      gpt_insert(rec_gpt, rec_base, leaf);
      tlb2_sync(rec_base, leaf.pte, rec_task_id);

      tree_ptr->parent = src->mt_ptr;
      tree_ptr->sister = ((mt_node_t *)tree_ptr->parent)->child;
      tree_ptr->child = 0;
      tree_ptr->vaddr = rec_base;
      tree_ptr->task = rec_task_id;
      
      ((mt_node_t *)tree_ptr->parent)->child = (w_t) tree_ptr;
      
    }
    else if ((dst->pte &  EL_Valid) == 0)
    {
      
      gpt_leaf_t leaf;
      mt_node_t *tree_ptr;
      
      /* add new node to the the mapping tree */
      
      tree_ptr = (mt_node_t *) mt_mem_alloc();


      /* receiver gpt invalid (empty) -> copy in new value */
      dst->mt_ptr = (w_t) tree_ptr;
      dst->pte = src->pte & read_write_mask;
      
      tlb2_sync(rec_base, dst->pte, rec_task_id);
      
      tree_ptr->parent = src->mt_ptr;
      tree_ptr->sister = ((mt_node_t *)tree_ptr->parent)->child;
      tree_ptr->child = 0;
      tree_ptr->vaddr = rec_base;
      tree_ptr->task = rec_task_id;
      
      ((mt_node_t *)tree_ptr->parent)->child = (w_t) tree_ptr;
    }
    else if((dst->pte &  EL_PFN) == (src->pte & EL_PFN))
    {
      /* receiver has same page mapped -> make attributes same */
      dst->pte = src->pte & read_write_mask;
      tlb2_sync(rec_base, dst->pte, rec_task_id);
    }
    else
    {
      /* receiver has different page mapped -> nop */
      return;
    }
  }
}

/****************************************************************************
 *
 * gpt_fpage_unmap: performs unmap operation to current and child address
 * spaces. Blows away mapping trees as it performs unmaps. Always succeeds
 * even if it does nothing.
 *
 * Inputs:
 * 	fpage: fpage to unmap
 * 	mask: how to unmap
 * 	gpt: gpt to do unmapping on
 *
 ****************************************************************************/

void gpt_fpage_unmap(udw_t fpage, dw_t mask, gpt_t *gpt)
{
  void *sp;
  ub_t s1, s2, i, level;
  udw_t base, vaddr, end;
  gpt_t *leaf;
  uw_t read_write_mask;

  sp = alloca((WORDLEN/GPTSIZE+1)*8);

  /* first decode args and check they are valid */
  
  s1 =  ((fpage & GPT_FPAGE_SIZE_MASK) >> GPT_FPAGE_SIZE_SHIFT);
  
  if (s1 < 12) s1 = 12;
  if (s1 > 63) s1 = 63;
  
  base = fpage & ~((1ul << s1) -1);
  
  if (base >=  MAXUSEG) return;
  
  end =  base - 1 + (1ul << s1) ;
  if (end > MAXUSEG) end = MAXUSEG - 1;

  /* now wander down the tree to where we start doing real work */
  
  vaddr = 0;
  i = base >> (WORDLEN-GPTSIZE);
  level = WORDLEN;
  
  do {
    leaf = &(gpt[i]);
    if (leaf->guard == NULL_GUARD(((udw_t)level),
				   ((udw_t)GPTSIZE),
				   ((udw_t)i)))
    {
      i++;
      break;
    }
    if (
	
	(
	 (~(udw_t)i & ((1ul << GPTSIZE) - 1ul)) 
	 == 
	 (
	  (leaf->guard >> (level - GPTSIZE)) & 
	  ((1ul << GPTSIZE) - 1ul)
	  )
	 ) ||
	(
	 ((1ul << GPTSIZE) + i)
	 ==
	 ((leaf->guard >> (level - GPTSIZE))
	  & ((1ul << (GPTSIZE + 1)) - 1ul))
	 )
	
	)
      
    {
      break;
    }
    else
    {
      s1 = leaf->guard & 077;
      s2 = WORDLEN-level+GPTSIZE;
      vaddr = (level == WORDLEN) ?
	((udw_t)i << (level - GPTSIZE)) |
	(leaf->guard >> s1 << s1 << s2 >> s2)
	: (vaddr >> level << level) | ((udw_t)i << (level - GPTSIZE)) |
	(leaf->guard >> s1 << s1 << s2 >> s2);
      push_pos(gpt,i+1,level,&sp);
      level = leaf->guard & 077;
      gpt = (gpt_t *)(leaf->u.ptr - 15);
      i = (base >> (level - GPTSIZE)) & (( 1ul << GPTSIZE) - 1);
    }
    
  } while (1);
  
  /* now we are near the beginning so we walk through the tree unmapping
     from here */

  push_pos(gpt,i,level,&sp);
  
  do
  {
    
    pop_pos(&gpt,&i,&level,&sp);
    while  (i < (1 << GPTSIZE))
    {
      leaf = &(gpt[i]);
      if (leaf->guard == NULL_GUARD(((udw_t)level),
				    ((udw_t)GPTSIZE),
				    ((udw_t)i)))
      {
	i++;
	if (
	    (vaddr = (vaddr >> level << level) |
	     ((udw_t)i << (level - GPTSIZE))) > end)
	{
	  break;
	}
      }
      else
      {
	s1 = leaf->guard & 077;
	s2 = WORDLEN-level+GPTSIZE;
	vaddr = (level == WORDLEN) ?
	  ((udw_t)i << (level - GPTSIZE)) |
	  (leaf->guard >> s1 << s1 << s2 >> s2)
	  : (vaddr >> level << level) | ((udw_t)i << (level - GPTSIZE)) |
	  (leaf->guard >> s1 << s1 << s2 >> s2);
	
	if (vaddr > end) break;
	
	if (
	    
	    (
	     (~(udw_t)i & ((1ul << GPTSIZE) - 1ul)) 
	     == 
	     (
	      (leaf->guard >> (level - GPTSIZE)) & 
	      ((1ul << GPTSIZE) - 1ul)
	      )
	     ) ||
	    (
	     ((1ul << GPTSIZE) + i)
	     ==
	     ((leaf->guard >> (level - GPTSIZE))
	      & ((1ul << (GPTSIZE + 1)) - 1ul))
	     )
	    
	    )
	{
	  
	  if (leaf->u.lf.pte & EL_Valid)
	  {
	    mt_node_t *start, *x;
	    void *t;
	    /* have to unmap children of pager */
	    start = (mt_node_t *) leaf->u.lf.mt_ptr;
	    
	    x = start;
	    if (mask & L4_FPAGE_RW_MASK)
	    {
	      while (x != start || x->child != 0)
	      {
		gpt_t * gpt_ptr;
		while (x->child != 0)
		{
		  x = (mt_node_t *) x->child;
		}
		((mt_node_t *)x->parent)->child = x->sister;
		
		/* invalidate gpt */
#ifndef SIM		
		gpt_ptr =* (gpt_t **) (((udw_t)(x->task)
					<< 36 >> 46 <<  TCB_SIZE_SHIFT)
				       + TCB_VBASE + T_GPT_POINTER);
#else
		gpt_ptr = vas[x->task].tcb.gpt_pointer;
#endif		  
		t = (void *) gpt_lookup_ptr(gpt_ptr, x->vaddr);
		if (t != 0)
		{
		  ((gpt_leaf_t *)t)->pte = 0;
		  tlb2_sync(x->vaddr, 0, x->task);
		}
		else
		{
		  panic("L4 PANIC: PTE not found, page table corrupt?"); /* ok */
		}
		
		t = (void *)x;
		x = (mt_node_t *) x->parent;
		mt_mem_free((mt_node_t *)t);
		
	      }
	    }
	    else
	    {
	      read_write_mask = ~ EL_Dirty;
	      
	      do {
		while (x->child != 0)
		  x = (mt_node_t *) x->child;
		while (x != start)
		{
		  gpt_t * gpt_ptr;
#ifndef SIM		  
		  gpt_ptr =* (gpt_t **) (((udw_t)(x->task)
					  << 36 >> 46 <<  TCB_SIZE_SHIFT)
					 + TCB_VBASE + T_GPT_POINTER);
#else
		  gpt_ptr = vas[x->task].tcb.gpt_pointer;
#endif		  
		  t = (void *) gpt_lookup_ptr(gpt_ptr, x->vaddr);
		  if (t != 0)
		  {
		    ((gpt_leaf_t *)t)->pte &= read_write_mask;
		    tlb2_sync(x->vaddr, ((gpt_leaf_t *)t)->pte, x->task);
		  }
		  else
		  {
		    panic("L4 PANIC: PTE not found, page table corrupt?"); /* ok */
		  }
		  
		  if (x->sister != 0)
		  {
		    x = (mt_node_t *) x->sister;
		    break;
		  }
		  else
		  {
		    x = (mt_node_t *) x->parent;
		  }
		}
	      } while (x != start);
	    }
	    
	    if (mask < 0)
	    {
	      if (mask & L4_FPAGE_RW_MASK)
	      {
		leaf->u.lf.pte = 0;
		tlb2_sync(start->vaddr, 0, start->task);
		
		
		/* remove start as well */
		if ( ((mt_node_t *)((mt_node_t *)start->parent)->child) == start)
		{
		  ((mt_node_t *)start->parent)->child = start->sister;
		}
		else
		{
		  t =(void  *) ((mt_node_t *)start->parent)->child;
		  while((mt_node_t *)((mt_node_t *)t)->sister != start)
		  {
		    t = (void *)(((mt_node_t *) t)->sister);
		  }
		  
		  ((mt_node_t *)t)->sister = start->sister;
		}
		mt_mem_free(start);
	      }
	      
	      else
	      {
		read_write_mask = ~ EL_Dirty;
		leaf->u.lf.pte &= read_write_mask;
		tlb2_sync(x->vaddr, leaf->u.lf.pte, x->task);
	      }
	    }
	  }
	  i++;
	}
	else
	{
	  push_pos(gpt,i+1,level,&sp);
	  level = leaf->guard & 077;
	  gpt = (gpt_t *)(leaf->u.ptr - 15);
	  i = 0;
	}
      }
    } 
  } while (vaddr < end && level != WORDLEN);
}

      
/****************************************************************************
 *
 * gpt_free_tree: goes through and frees an entire tree and returns it to
 * the gpt pool.
 * 	gpt: gpt to free
 *
 ****************************************************************************/

void gpt_free_tree(gpt_t *gpt)
{
  ub_t i, level, s1, s2;
  udw_t vaddr;
  void *sp;
  
  sp = alloca((WORDLEN/GPTSIZE+1)*8);
  vaddr = 0;
  push_pos(gpt,0,WORDLEN,&sp);
  do
  {
    pop_pos(&gpt,&i,&level,&sp);
    while  (i < (1 << GPTSIZE))
    {
      if (gpt[i].guard == NULL_GUARD(((udw_t)level),
				     ((udw_t)GPTSIZE),
				     ((udw_t)i)))
      {
	i++;
      }
      else
      {
	if (
	    
	    (
	     (~(udw_t)i & ((1ul << GPTSIZE) - 1ul)) 
	     == 
	     (
	      (gpt[i].guard >> (level - GPTSIZE)) & 
	      ((1ul << GPTSIZE) - 1ul)
	      )
	     ) ||
	    (
	     ((1ul << GPTSIZE) + i)
	     ==
	     ((gpt[i].guard >> (level - GPTSIZE))
	      & ((1ul << (GPTSIZE + 1)) - 1ul))
	     )
	    
	    )
	{
	  i++;
	}
	else
	{
	  push_pos(gpt,i+1,level,&sp);
	  level = gpt[i].guard & 077;
	  gpt = (gpt_t *)(gpt[i].u.ptr - 15);
	  i = 0;
	}
      }
    }
    gpt_mem_free(gpt);
  } while (level != WORDLEN);
}

void task_tcb_unmap(udw_t base, gpt_t *gpt)
{
  void *sp;
  ub_t s1, s2, i, level;
  udw_t vaddr, end;
  gpt_t *leaf;
  uw_t read_write_mask;

  sp = alloca((WORDLEN/GPTSIZE+1)*8);

  end =  base - 1 + ( MAX_THREADS * TCB_SIZE) ;

  /* now wander down the tree to where we start doing real work */
  
  vaddr = 0;
  i = base >> (WORDLEN-GPTSIZE);
  level = WORDLEN;
  
  do {
    leaf = &(gpt[i]);
    if (leaf->guard == NULL_GUARD(((udw_t)level),
				   ((udw_t)GPTSIZE),
				   ((udw_t)i)))
    {
      i++;
      break;
    }
    if (
	
	(
	 (~(udw_t)i & ((1ul << GPTSIZE) - 1ul)) 
	 == 
	 (
	  (leaf->guard >> (level - GPTSIZE)) & 
	  ((1ul << GPTSIZE) - 1ul)
	  )
	 ) ||
	(
	 ((1ul << GPTSIZE) + i)
	 ==
	 ((leaf->guard >> (level - GPTSIZE))
	  & ((1ul << (GPTSIZE + 1)) - 1ul))
	 )
	
	)
      
    {
      break;
    }
    else
    {
      s1 = leaf->guard & 077;
      s2 = WORDLEN-level+GPTSIZE;
      vaddr = (level == WORDLEN) ?
	((udw_t)i << (level - GPTSIZE)) |
	(leaf->guard >> s1 << s1 << s2 >> s2)
	: (vaddr >> level << level) | ((udw_t)i << (level - GPTSIZE)) |
	(leaf->guard >> s1 << s1 << s2 >> s2);
      push_pos(gpt,i+1,level,&sp);
      level = leaf->guard & 077;
      gpt = (gpt_t *)(leaf->u.ptr - 15);
      i = (base >> (level - GPTSIZE)) & (( 1ul << GPTSIZE) - 1);
    }
    
  } while (1);
  
  /* now we are near the beginning so we walk through the tree unmapping
     from here */

  push_pos(gpt,i,level,&sp);
  
  do
  {
    
    pop_pos(&gpt,&i,&level,&sp);
    while  (i < (1 << GPTSIZE))
    {
      leaf = &(gpt[i]);
      if (leaf->guard == NULL_GUARD(((udw_t)level),
				    ((udw_t)GPTSIZE),
				    ((udw_t)i)))
      {
	i++;
	if (
	    (vaddr = (vaddr >> level << level) |
	     ((udw_t)i << (level - GPTSIZE))) > end)
	{
	  break;
	}
      }
      else
      {
	s1 = leaf->guard & 077;
	s2 = WORDLEN-level+GPTSIZE;
	vaddr = (level == WORDLEN) ?
	  ((udw_t)i << (level - GPTSIZE)) |
	  (leaf->guard >> s1 << s1 << s2 >> s2)
	  : (vaddr >> level << level) | ((udw_t)i << (level - GPTSIZE)) |
	  (leaf->guard >> s1 << s1 << s2 >> s2);
	
	if (vaddr > end) break;
	
	if (
	    
	    (
	     (~(udw_t)i & ((1ul << GPTSIZE) - 1ul)) 
	     == 
	     (
	      (leaf->guard >> (level - GPTSIZE)) & 
	      ((1ul << GPTSIZE) - 1ul)
	      )
	     ) ||
	    (
	     ((1ul << GPTSIZE) + i)
	     ==
	     ((leaf->guard >> (level - GPTSIZE))
	      & ((1ul << (GPTSIZE + 1)) - 1ul))
	     )
	    
	    )
	{
	  if (((leaf->u.lf.pte & EL_PFN) << 6) != INVALID_TCB_BASE)
	  {
	    if (((leaf->u.lf.pte & EL_PFN) << 6) == 0)
	    {
	      panic("L4 PANIC: found invalid PTE in tcb space"); /* ok */
	    }
	    
	    tcb_frame_free((void *)vaddr,
			   (void *) ((leaf->u.lf.pte & EL_PFN) << 6));
	    leaf->u.lf.pte = INVALID_TCB_BASE >>  FRAME_SIZE_SHIFT << 6 |
	      EL_Cache_CNWb |  EL_Valid | EL_Global;
	    
	    tlb2_sync_shared(vaddr, leaf->u.lf.pte);
	  }
	  i++;
	}
	else
	{
	  push_pos(gpt,i+1,level,&sp);
	  level = leaf->guard & 077;
	  gpt = (gpt_t *)(leaf->u.ptr - 15);
	  i = 0;
	}
      }
    } 
  } while (vaddr < end && level != WORDLEN);
}


gpt_leaf_t *spi_leaf, spi_entry;
udw_t size, spi_odd_even;
mt_node_t *tree_ptr;
kernel_vars *spi_k;

void sigma0_pt_insert(gpt_t *sigma0_gpt, udw_t vaddr, w_t pte)
{
  
  spi_k = KERNEL_BASE;
  
  
  /* first insert pte in gpt */
  
  spi_leaf = gpt_lookup_ptr(sigma0_gpt, vaddr);

  if (spi_leaf == 0)
  {
    spi_entry.pte = pte;
    spi_leaf = gpt_insert(sigma0_gpt, vaddr, spi_entry);
#ifndef SIM    
    tlb2_sync(vaddr, pte, (w_t) (SIGMA0_TID).ID);
#else
    tlb2_sync(vaddr, pte, 0);
#endif
    if (spi_leaf == 0)
    {
      panic("L4 PANIC: Sigma0 failed to insert PTE"); /* ok */
    }
  }
  else
  {
    if (((spi_leaf->pte) & EL_Valid) == 0)
    {
      spi_leaf->pte = pte;
      
#ifndef SIM    
    tlb2_sync(vaddr, pte, (w_t) (SIGMA0_TID).ID);
#else
    tlb2_sync(vaddr, pte, 0);
#endif
    }
  }
  
  /* put entry in top of a mapping tree */

  tree_ptr = mt_mem_alloc();
  
  spi_leaf->mt_ptr = (w_t) tree_ptr;
  tree_ptr->parent = 0;
  tree_ptr->sister = 0;
  tree_ptr->child = 0;
  tree_ptr->vaddr = vaddr;
#ifndef SIM
  tree_ptr->task = (w_t) (SIGMA0_TID).ID;
#else
  tree_ptr->task = 0;
#endif  

}

extern char outbuff[];
  

void gpt_print(gpt_t *gpt)
{
  unsigned char i;
  unsigned char level;
  udw_t vaddr;
  unsigned char s1, s2;
  void *sp;
  sp = alloca((WORDLEN/GPTSIZE+1)*8);
  vaddr = 0;
  push_pos(gpt,0,WORDLEN,&sp);
  do
  {
    pop_pos(&gpt,&i,&level,&sp);
    while  (i < (1 << GPTSIZE))
    {
      if (gpt[i].guard == NULL_GUARD(((udw_t)level),
				     ((udw_t)GPTSIZE),
				     ((udw_t)i)))
      {
	i++;
      }
      else
      {
	s1 = gpt[i].guard & 077;
	s2 = WORDLEN-level+GPTSIZE;
	vaddr = (level == WORDLEN) ?
	  ((udw_t)i << (level - GPTSIZE)) |
	  (gpt[i].guard >> s1 << s1 << s2 >> s2)
	  : (vaddr >> level << level) | ((udw_t)i << (level - GPTSIZE)) |
	  (gpt[i].guard >> s1 << s1 << s2 >> s2);
	
	if (
	    
	    (
	     (~(udw_t)i & ((1ul << GPTSIZE) - 1ul)) 
	     == 
	     (
	      (gpt[i].guard >> (level - GPTSIZE)) & 
	      ((1ul << GPTSIZE) - 1ul)
	      )
	     ) ||
	    (
	     ((1ul << GPTSIZE) + i)
	     ==
	     ((gpt[i].guard >> (level - GPTSIZE))
	      & ((1ul << (GPTSIZE + 1)) - 1ul))
	     )
	    
	    )
	{
	  sprintf(outbuff, "v = 0x%llx pte = 0x%llx\r\n",
		  vaddr,
		  (udw_t) gpt[i].u.lf.pte << 6
		  );
	  printbuff();
	  i++;
	}
	else
	{
	  push_pos(gpt,i+1,level,&sp);
	  level = gpt[i].guard & 077;
	  gpt = (gpt_t *)(gpt[i].u.ptr - 15);
	  i = 0;
	}
      }
    } 
  } while (level != WORDLEN);
}


void gpt_print_fpage(gpt_t *gpt, udw_t base, ub_t size)
{
  void *sp;
  udw_t end, vaddr;
  ub_t s1, s2, i, level;
  sp = alloca((WORDLEN/GPTSIZE+1)*8);

  if (size < 12)
  {
    size = 12;
  }
  if (size > 63)
  {
    size = 63;
  }
  
  base = base & (~((1ul << size) -1));
  end = base - 1 + (1ul << size) ;
  
  vaddr = 0;
  i = base >> (WORDLEN-GPTSIZE);
  level = WORDLEN;
  
  do {
    if (gpt[i].guard == NULL_GUARD(((udw_t)level),
				   ((udw_t)GPTSIZE),
				   ((udw_t)i)))
    {
      i++;
      break;
    }
    if (
	
	(
	 (~(udw_t)i & ((1ul << GPTSIZE) - 1ul)) 
	 == 
	 (
	  (gpt[i].guard >> (level - GPTSIZE)) & 
	  ((1ul << GPTSIZE) - 1ul)
	  )
	 ) ||
	(
	 ((1ul << GPTSIZE) + i)
	 ==
	 ((gpt[i].guard >> (level - GPTSIZE))
	  & ((1ul << (GPTSIZE + 1)) - 1ul))
	 )
	
	)
      
    {
      break;
    }
    else
    {
      s1 = gpt[i].guard & 077;
      s2 = WORDLEN-level+GPTSIZE;
      vaddr = (level == WORDLEN) ?
	((udw_t)i << (level - GPTSIZE)) |
	(gpt[i].guard >> s1 << s1 << s2 >> s2)
	: (vaddr >> level << level) | ((udw_t)i << (level - GPTSIZE)) |
	(gpt[i].guard >> s1 << s1 << s2 >> s2);
      push_pos(gpt,i+1,level,&sp);
      level = gpt[i].guard & 077;
      gpt = (gpt_t *)(gpt[i].u.ptr - 15);
      i = (base >> (level - GPTSIZE)) & (( 1ul << GPTSIZE) - 1);
    }
    
  } while (1);

  /* we can assume we are either at the base, or above it, if it's invalid */


  
  push_pos(gpt,i,level,&sp);

  do
  {
    pop_pos(&gpt,&i,&level,&sp);
    while  (i < (1 << GPTSIZE))
    {
      if (gpt[i].guard == NULL_GUARD(((udw_t)level),
				     ((udw_t)GPTSIZE),
				     ((udw_t)i)))
      {
	i++;
	if (
	    (vaddr = (vaddr >> level << level) |
	     ((udw_t)i << (level - GPTSIZE))) > end)
	{
	  break;
	}
      }
      else
      {
	s1 = gpt[i].guard & 077;
	s2 = WORDLEN-level+GPTSIZE;
	vaddr = (level == WORDLEN) ?
	  ((udw_t)i << (level - GPTSIZE)) |
	  (gpt[i].guard >> s1 << s1 << s2 >> s2)
	  : (vaddr >> level << level) | ((udw_t)i << (level - GPTSIZE)) |
	  (gpt[i].guard >> s1 << s1 << s2 >> s2);
	
	if (vaddr > end) break;
	
	if (
	    
	    (
	     (~(udw_t)i & ((1ul << GPTSIZE) - 1ul)) 
	     == 
	     (
	      (gpt[i].guard >> (level - GPTSIZE)) & 
	      ((1ul << GPTSIZE) - 1ul)
	      )
	     ) ||
	    (
	     ((1ul << GPTSIZE) + i)
	     ==
	     ((gpt[i].guard >> (level - GPTSIZE))
	      & ((1ul << (GPTSIZE + 1)) - 1ul))
	     )
	    
	    )
	{
	  sprintf(outbuff, "v = 0x%llx pte = 0x%llx\r\n",
		  vaddr,
		  (udw_t) gpt[i].u.lf.pte << 6
		  );
	  printbuff();
	  i++;
	}
	else
	{
	  push_pos(gpt,i+1,level,&sp);
	  level = gpt[i].guard & 077;
	  gpt = (gpt_t *)(gpt[i].u.ptr - 15);
	  i = 0;
	}
      }
    } 
  } while (vaddr < end && level != WORDLEN);
}

