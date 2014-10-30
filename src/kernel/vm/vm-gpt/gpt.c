/****************************************************************************
 *      $Id: gpt.c,v 1.7 1999/09/11 05:55:07 gernot Exp $
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
#include "gpt.h"
#include <kernel/panic.h>
#include <kernel/kmem.h>
#include <kernel/trace.h>
#include <kernel/vm.h>
#include <kernel/kernel.h>
#include <l4/types.h>
#include <l4/sigma0.h>
#include <alloca.h>
#ifdef SIM
#include <sim.h>
#include <stdio.h>
#endif


/****************************************************************************
 *
 * gpt_init: Create and initialise the top node of the GPT tree
 *
 ****************************************************************************/

gpt_t *gpt_init(void)
{
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
  return (gpt_t *)((udw_t)gpt | 15);
}


gpt_leaf_t *gpt_insert(tcb_t *tcb, udw_t vaddr)
{
  udw_t ptr;
  gpt_t *optr;
  udw_t guard, oguard;
  udw_t level, v, size;
#ifdef KDEBUG
	  alphn_set("gpti");
#endif	  

  /* walk down tree */
  v = vaddr;                       /* initial vaddr */
/****************************************************************************
 * ugly hack -- if called with bit 0 set, it's a gpt pointer, else it's
 * a tcb pointer 
 ****************************************************************************/
  if ((udw_t)tcb & 1)  
  {
    ptr = (dword_t) tcb;
    tcb = 0;
  }
  else
  {
    ptr = (dword_t) tcb->gpt_pointer;    /* 'aligned' pointer */
  }
#ifdef _SIM  
  printf("insert: vaddr 0x%llx ptr 0x%llx\n", vaddr, ptr);
#endif  
  
  guard = ((WORDLEN - GPTROOTSIZE - 4) << 6);  /* initial level */
  
  do {
    level = (guard >> 6) & 0x3f;
    size = v >> level;                 /* calc offset into current gpt */
    ptr = ptr | size;                  /* add to pointer */
    oguard = guard;                    /* remember guard from above */  
    guard = *(dword_t *)(ptr - 15);    /* load guard */
    optr = (gpt_t *) ptr;              /* remember ptr to current entry */
    ptr = *(dword_t *)(ptr - 7);       /* load pointer to next level */
    v = v ^ guard;                     /* xor current v with guard */
    level = v >> (guard & 0x3f);       /* shift to check guard zero */
  } while (level == 0);
  
  
  
  /* figure out if match or mismatch */
  if ((level << ((guard >> 6) & 0x3f)) == 0)
  {
    /* we have a match */
    return &((gpt_t *) ((udw_t) optr - 15))->u.lf;
  }
  
  /* no match, need to figure out what is going on and insert an entry */
  
  
  /* figure out where we are in tree */
  optr = (gpt_t *)((udw_t) optr - 15);
  level = oguard & 0x3f;
  if (level == 0)
  {
    size = GPTROOTSIZE;
    level = 64;
  }
  else
  {
    size = GPTSIZE;
  }
#ifdef _SIM  
  printf("insert: level %d size %d  optr 0x%llx guard 0x%llx\n",
	 level, size, optr, guard);
#endif  
  /* first check if we hit a null guard */
  if (guard ==
      (((~vaddr >> (level - size)) & ((1ul << size) -1)) << (level - size)))
  {
    /* yes we have a null guard, simply add a leaf and fix guard */
    udw_t s0, glen, ubar, g;
    
    optr->u.lf.pte = 0;
    glen = level - size - FRAME_SIZE_SHIFT;
    
    if (glen == 0)
    {
      /* encode zero length leaf guards differently */
      s0 = WORDLEN - size;
      ubar = (1 << size) | ((vaddr >> (level - size)) & ((1 << size) -1));
    }
    else
    {
      s0 = WORDLEN - glen;
      ubar = (~vaddr >> (level - size)) & ((1 << size) -1);
    }
    g = (vaddr >> FRAME_SIZE_SHIFT) & ((1ul << glen) - 1ul);
    
    optr->guard = 
      (((ubar << glen) + g) << FRAME_SIZE_SHIFT ) + (s0 << 6ul) + FRAME_SIZE_SHIFT;
#if 0 
    printf("insert: null: guard 0x%llx s0 %d s1 %d\n",
	   optr->guard, s0, FRAME_SIZE_SHIFT);
#endif    
    return &optr->u.lf;
  }
  else
  {
    gpt_t *new_node;
    udw_t glen, nu, ng, s0, s1;
    /* we have a guard clash and need to split */
    
    s1 = guard & 077ul;
    glen = level - size - s1;
    
    /* see how much of the guard matches */
    nu = guard >> s1 & ((1ul << glen) - 1ul);
    ng = vaddr >> s1 & ((1ul << glen) - 1ul);
    
    while (glen > 0)
    {
      if (ng != nu)
      {
	ng = ng >> GPTSIZE;
	nu = nu >> GPTSIZE;
	glen -= GPTSIZE;
      }
      else
      {
	break;
      }
    }
    
    new_node = gpt_mem_alloc(); /* assume gpt_mem_alloc panics if mem
				   unavailable */
#ifdef TLB_INSTR
    if(tcb)
    {
      tcb->pt_size += (1ul << GPTSIZE) * 16;
    }
#endif    
    for (nu = 0; nu < 1ul << GPTSIZE; nu++)
    {
      new_node[nu].guard =
	NULL_GUARD(level - size - glen, GPTSIZE, nu);
      new_node[nu].u.ptr = 0;
    }
    
    nu = (guard  >> (level - size - glen - GPTSIZE)) &
      ((1ul << GPTSIZE) - 1ul);
    ng = guard >> s1 &
      ((1ul << (level - size - glen - GPTSIZE - s1)) - 1ul);
    new_node[nu].u.lf = optr->u.lf;
    
    if (
	(((~vaddr >> (level - size)) & ((1ul << size) -1ul))) ==
	(guard >> (level - size) & ((1ul << size) -1ul))
	||
	((1 << size) | (vaddr >> (level - size)) & ((1 << size) -1)) ==
	((guard >> (level - size)) & ((1ul << (size+1)) -1ul))
	)
      
    {
      udw_t tmpu;
#if 0      
      printf("insert: lower leaf\n");
#endif      
      /* lower node is a leaf
       */
      if ((level-size-glen-GPTSIZE-s1) == 0)
      {
	s0 = WORDLEN - GPTSIZE;
	tmpu = (1ul << GPTSIZE) + nu;
      }
      else
      {
	tmpu = (~nu & ((1ul << GPTSIZE) - 1ul));
	s0 = (WORDLEN - (level-size-glen-GPTSIZE-FRAME_SIZE_SHIFT));
      }
      
      /* fix lower node */
      new_node[nu].guard =
	(((tmpu
	   << (level-size-glen-GPTSIZE-FRAME_SIZE_SHIFT)) + ng) << FRAME_SIZE_SHIFT)
	+ (s0 << 6ul) + FRAME_SIZE_SHIFT;
      
    }
    else
    {
      /* lower node not a leaf */
#if 0
      printf("insert: lower node\n");
#endif
      new_node[nu].guard =
	((nu << (level - size - glen - GPTSIZE - s1)) + ng)
	     << s1 | (guard & 07777ul);
    }
#if 0    
    printf("insert: split: ou 0x%llx guard 0x%llx s0 %d s1 %d\n",
	   nu, new_node[nu].guard, (guard >> 6) & 077, s1);
#endif
    
    /* fix old upper new node */
    s1 = level - size - glen;
    nu = (vaddr >> (level - size)) & ((1 << size) -1);
    ng = guard >> s1 & ((1ul << glen) -1ul);
    optr->u.ptr = ((udw_t ) new_node) + 15ul;
    optr->guard = (((nu << glen) + ng) << s1) +
      ((s1 - GPTSIZE - 4) << 6ul) + s1;

    /* now add new entry in new node */

    nu = (vaddr >> (s1 - GPTSIZE)) & ((1ul << GPTSIZE) - 1ul);
    glen = s1 - GPTSIZE - FRAME_SIZE_SHIFT;
    size = GPTSIZE;
    if (glen == 0)
    {
      /* encode zero length leaf guards differently */
      s0 = WORDLEN - size;
      v = (1 << size) | (nu & ((1 << size) -1));
    }
    else
    {
      s0 = WORDLEN - glen;
      v = (~nu) & ((1 << size) -1);
    }
    
    ng = (vaddr >> FRAME_SIZE_SHIFT) & ((1ul << glen) - 1ul);
    new_node[nu].guard =
      (((v << glen) + ng) << FRAME_SIZE_SHIFT ) + (s0 << 6ul) + FRAME_SIZE_SHIFT;
    
    new_node[nu].u.lf.pte = 0;
    
    return &new_node[nu].u.lf;
  }
  
}

void vm_map(tcb_t *snd_tcb, dword_t snd_fpage, dword_t snd_cp,
                   tcb_t *rcv_tcb, dword_t rec_fpage)
{
  
  udw_t snd_base ,end;
  uw_t read_write_mask;
  dw_t align;

  
#ifdef KDEBUG
	  alphn_set("vmap");
#endif	  
   
  {
    udw_t snd_size_mask, rec_base;
    udw_t rec_size_mask, map_size;

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
    
    if (snd_base >= MAXUSEG || rec_base >= MAXUSEG) return;
    
    map_size = (snd_size_mask & rec_size_mask) + 1;
    
    if (snd_base + map_size >  MAXUSEG)
    {
      map_size = MAXUSEG - snd_base;
    }
    
    if (rec_base + map_size > MAXUSEG)
    {
      map_size = MAXUSEG - rec_base;
    }
    
    end = snd_base + map_size;
    align = rec_base - snd_base;
    
#ifdef _SIM
    printf("vm_map: sbase 0x%llx rbase 0x%llx msize 0x%llx\n",
	   snd_base, rec_base, map_size);
    fflush(stdout);
#endif
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
  
  /* now walk down snd tree */

  {
    udw_t   vaddr;
    unsigned char level, s1, s2, size;
    unsigned short i;
    void *sp;
    gpt_leaf_t *src, *dst;
    gpt_t *snd_gpt, *gptr;
    
    sp = alloca((WORDLEN/GPTSIZE+1)*8);
    vaddr = 0;
    i = snd_base >> (WORDLEN-GPTROOTSIZE);
    level = WORDLEN;
    snd_gpt = (gpt_t *)(((udw_t) snd_tcb->gpt_pointer) - 15);
    size = GPTROOTSIZE;
    
    do {
#ifdef _SIM
      printf("vm_map: vaddr %llx level %d size %d i %d\n",
	     vaddr, level, size, i);
      fflush(stdout);
#endif
      gptr = &(snd_gpt[i]);
      if (gptr->guard == NULL_GUARD(((udw_t)level),
				    ((udw_t)size),
				    ((udw_t)i)))
      {
	i++;
	break;
      }
      if (
	  
	  (
	   (~(udw_t)i & ((1ul << size) - 1ul)) 
	   == 
	   (
	    (gptr->guard >> (level - size)) & 
	    ((1ul << size) - 1ul)
	    )
	   ) ||
	  (
	   ((1ul << size) + i)
	   ==
	   ((gptr->guard >> (level - size))
	    & ((1ul << (size + 1)) - 1ul))
	   )
	  
	  )
	
      {
	break;
      }
      else
      {
	push_pos(snd_gpt,i+1,level,size,&sp);
	
	s1 = gptr->guard & 077;
	s2 = WORDLEN-level+size;
	
	vaddr = (level == WORDLEN) ?
	  
	  ((udw_t)i << (level - size)) |
	  (gptr->guard >> s1 << s1 << s2 >> s2)
	  
	: (vaddr >> level << level) | ((udw_t)i << (level - size)) |
	  (gptr->guard >> s1 << s1 << s2 >> s2);
	
	level = s1;
	size = GPTSIZE;
	snd_gpt = (gpt_t *)(gptr->u.ptr - 15);
	i = (snd_base >> (level - size)) & (( 1ul << size) - 1);
      }
    } while (1);
    
    
    push_pos(snd_gpt,i,level,size,&sp);
    
    do
    {
      
      pop_pos(&snd_gpt,&i,&level,&size,&sp);
      while  (i < (1 << size))
      {
	gptr = &(snd_gpt[i]);
	if (gptr->guard == NULL_GUARD(((udw_t)level),
				      ((udw_t)size),
				      ((udw_t)i)))
	{
	  i++;
	  if (
	      (vaddr = (vaddr >> level << level) |
	       ((udw_t)i << (level - size))) >= end)
	  {
	    break;
	  }
	}
	else
	{
	  s1 = gptr->guard & 077;
	  s2 = WORDLEN-level+size;
	  vaddr = (level == WORDLEN) ?
	    ((udw_t)i << (level - size)) |
	    (gptr->guard >> s1 << s1 << s2 >> s2)
	    : (vaddr >> level << level) | ((udw_t)i << (level - size)) |
	    (gptr->guard >> s1 << s1 << s2 >> s2);
	  
	  if (vaddr >= end) break;
	  
	  if (
	      
	      (
	       (~(udw_t)i & ((1ul << size) - 1ul)) 
	       == 
	       (
		(gptr->guard >> (level - size)) & 
		((1ul << size) - 1ul)
		)
	       ) ||
	      (
	       ((1ul << size) + i)
	       ==
	       ((gptr->guard >> (level - size))
		& ((1ul << (size + 1)) - 1ul))
	       )
	      
	      )
	  {
	    if (vaddr >= snd_base)
	    {
#ifdef _SIM
	      printf("vm_map: vaddr %llx level %d size %d i %d\n",
		     vaddr, level, size, i);
	      printf("vm_map: pte 0x%x\n",
		     gptr->u.lf.pte);
	      fflush(stdout);
#endif
	      src = &gptr->u.lf;
	      
	      /* map pte0 */
	      if (src->pte & EL_Valid)
	      {
		udw_t dst_addr;
		w_t *mt_ptr;
		uw_t *pte;
		
		dst_addr = vaddr+align;
		
		/* get leaf in dest */
		dst = gpt_insert(rcv_tcb, dst_addr);
#ifdef KDEBUG
		alphn_set("vmp2");
#endif	  
		/* get pte */
		pte = &dst->pte;
		mt_ptr = &dst->mt_ptr;
		
		if (!(*pte & EL_Valid))
		{
		  mt_node_t *tree_ptr;
		  tree_ptr = (mt_node_t *) mt_mem_alloc();
		  
		  *mt_ptr = (w_t)tree_ptr;
		  *pte = src->pte & read_write_mask;
#ifdef TLB_INSTR
		  rcv_tcb->pt_number++;
#endif		  
		  tlb_sync(rcv_tcb, dst_addr, *pte);
		  
		  tree_ptr->parent = src->mt_ptr;
		  tree_ptr->sister = ((mt_node_t *)tree_ptr->parent)->child;
		  tree_ptr->child = 0;
		  tree_ptr->vaddr = dst_addr;
		  tree_ptr->task = rcv_tcb->myself;
		  ((mt_node_t *)tree_ptr->parent)->child = (w_t) tree_ptr;
		}
		else
		{
		  if ((*pte &  EL_PFN) == (src->pte & EL_PFN))
		  {
		    *pte = src->pte & read_write_mask;
		    tlb_sync(rcv_tcb, dst_addr, *pte);
		  }
		  /* else we already have a mapping, leave as is */
		}
	      }
	    }
	    i++;
	  }
	  else
	  {
	    push_pos(snd_gpt,i+1,level,size,&sp);
	    level = gptr->guard & 077;
	    snd_gpt = (gpt_t *)(gptr->u.ptr - 15);
	    size = GPTSIZE;
	    i = 0;
	  }
	}
      } 
    } while (vaddr < end && level != WORDLEN);
  }
}

extern char outbuff[];

void gpt_print(gpt_t *gpt)
{
  unsigned char dummy;
  unsigned short i;
  unsigned char level;
  udw_t vaddr;
  unsigned char s1, s2, size;
  void *sp;
  unsigned int depth, depth_sum, leaf, entries, guard;

  depth_sum = entries = leaf = guard = 0;
  depth = 1;
  gpt = (gpt_t *) ((udw_t) gpt - 15); 
  
  sp = alloca((WORDLEN/GPTSIZE+1)*8);
  vaddr = 0;
  size = GPTROOTSIZE;
  push_pos(gpt,0,WORDLEN,size,&sp);
  depth ++;
  do
  {
    pop_pos(&gpt,&i,&level,&size,&sp);
    depth --;
    while  (i < (1 << size))
    {
#if SIM      
      printf("gpt 0x%llx level %d i %d guard 0x%llx s0 %d s1 %d\n",
	     gpt, level, i, gpt[i].guard
	     , (gpt[i].guard >> 6) & 077, gpt[i].guard & 077);
#endif
      entries ++;
      if (gpt[i].guard == NULL_GUARD(((udw_t)level),
				     ((udw_t)size),
				     ((udw_t)i)))
      {
	
	i++;
      }
      else
      {
	s1 = gpt[i].guard & 077;
	s2 = WORDLEN-level+size;
	vaddr = (level == WORDLEN) ?
	  ((udw_t)i << (level - size)) |
	  (gpt[i].guard >> s1 << s1 << s2 >> s2)
	  : (vaddr >> level << level) | ((udw_t)i << (level - size)) |
	  (gpt[i].guard >> s1 << s1 << s2 >> s2);
	
	if (
	    
	    (
	     (~(udw_t)i & ((1ul << size) - 1ul)) 
	     == 
	     (
	      (gpt[i].guard >> (level - size)) & 
	      ((1ul << size) - 1ul)
	      )
	     ) ||
	    (
	     ((1ul << size) + i)
	     ==
	     ((gpt[i].guard >> (level - size))
	      & ((1ul << (size + 1)) - 1ul))
	     )
	    
	    )
	{
	  leaf ++;
	  depth_sum += depth;
#if 1	  
	  sprintf(outbuff, "v = 0x%llx pte = 0x%llx \r\n",
		  vaddr, (udw_t) gpt[i].u.lf.pte << 6);
	  printbuff();
#endif	  
	  i++;
	}
	else
	{
	  guard++;
	  push_pos(gpt,i+1,level,size,&sp);
	  depth++;
	  level = gpt[i].guard & 077;
	  gpt = (gpt_t *)(gpt[i].u.ptr - 15);
	  i = 0;
	  size = GPTSIZE;
	}
      }
    } 
  } while (level != WORDLEN);
  sprintf(outbuff, "entries = %d leaf = %d  guard = %d\n\r",
	  entries, leaf, guard);
  printbuff();

  sprintf(outbuff, "depth sum = %d\n\r",
	  depth_sum);
  printbuff();
}

void vm_fpage_unmap(tcb_t *tcb, dword_t fpage, dword_t mask)
{
  void *sp;
  ub_t s1, s2, level, size;
  unsigned short i;
  udw_t base, vaddr, end;
  gpt_t *gpt, *gptr;
  uw_t read_write_mask;

  sp = alloca((WORDLEN/GPTSIZE+1)*8);
  /* first decode args and check they are valid */
  
  s1 =  ((fpage & GPT_FPAGE_SIZE_MASK) >> GPT_FPAGE_SIZE_SHIFT);
  
  if (s1 < 12) s1 = 12;
  if (s1 > 63) s1 = 63;
  
  base = fpage & ~((1ul << s1) -1);
  
  if (base >=  MAXUSEG) return;
  
  end =  base + (1ul << s1) ;
  if (end > MAXUSEG) end = MAXUSEG;

  /* now wander down the tree to where we start doing real work */
  
  vaddr = 0;
  i = base >> (WORDLEN-GPTROOTSIZE);
  size = GPTROOTSIZE;
  level = WORDLEN;
  gpt = (gpt_t *)((udw_t)(tcb->gpt_pointer) - 15);
  
  do {
    gptr = &(gpt[i]);
    if (gptr->guard == NULL_GUARD(((udw_t)level),
				   ((udw_t)size),
				   ((udw_t)i)))
    {
      i++;
      break;
    }
    if (
	
	(
	 (~(udw_t)i & ((1ul << size) - 1ul)) 
	 == 
	 (
	  (gptr->guard >> (level - size)) & 
	  ((1ul << size) - 1ul)
	  )
	 ) ||
	(
	 ((1ul << size) + i)
	 ==
	 ((gptr->guard >> (level - size))
	  & ((1ul << (size + 1)) - 1ul))
	 )
	
	)
      
    {
      break;
    }
    else
    {
      push_pos(gpt,i+1,level,size,&sp);
      s1 = gptr->guard & 077;
      s2 = WORDLEN-level+size;
      vaddr = (level == WORDLEN) ?
	((udw_t)i << (level - size)) |
	(gptr->guard >> s1 << s1 << s2 >> s2)
	: (vaddr >> level << level) | ((udw_t)i << (level - size)) |
	(gptr->guard >> s1 << s1 << s2 >> s2);
      size = GPTSIZE;
      level = gptr->guard & 077;
      gpt = (gpt_t *)(gptr->u.ptr - 15);
      i = (base >> (level - size)) & (( 1ul << size) - 1);
    }
    
  } while (1);
  
  /* now we are near the beginning so we walk through the tree unmapping
     from here */

  push_pos(gpt,i,level,size,&sp);
  
  do
  {
    
    pop_pos(&gpt,&i,&level,&size,&sp);
    while  (i < (1 << size))
    {
      gptr = &(gpt[i]);
      if (gptr->guard == NULL_GUARD(((udw_t)level),
				    ((udw_t)size),
				    ((udw_t)i)))
      {
	i++;
	if (
	    (vaddr = (vaddr >> level << level) |
	     ((udw_t)i << (level - size))) >= end)
	{
	  break;
	}
      }
      else
      {
	s1 = gptr->guard & 077;
	s2 = WORDLEN-level+size;
	vaddr = (level == WORDLEN) ?
	  ((udw_t)i << (level - size)) |
	  (gptr->guard >> s1 << s1 << s2 >> s2)
	  : (vaddr >> level << level) | ((udw_t)i << (level - size)) |
	  (gptr->guard >> s1 << s1 << s2 >> s2);
	
	if (vaddr >= end) break;
	
	if (
	    
	    (
	     (~(udw_t)i & ((1ul << size) - 1ul)) 
	     == 
	     (
	      (gptr->guard >> (level - size)) & 
	      ((1ul << size) - 1ul)
	      )
	     ) ||
	    (
	     ((1ul << size) + i)
	     ==
	     ((gptr->guard >> (level - size))
	      & ((1ul << (size + 1)) - 1ul))
	     )
	    
	    )
	{
	  if (vaddr >= base)
	  {
	    pte_t *pte;
	    mt_node_t *mt_ptr;
#ifdef SIM
	    printf("vm_unmap: unmaping vaddr 0x%llx\n", vaddr);
#endif	  
	    /* do unmaping */
	    pte = & (gptr->u.lf.pte);
	    mt_ptr = (mt_node_t *) (gptr->u.lf.mt_ptr);
	    if (*pte & EL_Valid)
	    {
	      mt_node_t *start, *x;
	      void *t;
	      /* have to unmap children of pager */
	      start = mt_ptr;
	      
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
		  t = (void *) vm_lookup_pte(gpt_ptr, x->vaddr);
		  if (t != 0)
		  {
		    *(pte_t *)t = 0;
#ifdef SIM		      
		    tlb_sync(&vas[x->task].tcb, x->vaddr, 0);
#else
		    tlb_sync((tcb_t *)
			     (((udw_t)(x->task)
			       << 36 >> 46 <<  TCB_SIZE_SHIFT)
			      + TCB_VBASE), x->vaddr, 0);
#endif		    
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
		    t = (void *) vm_lookup_pte(gpt_ptr, x->vaddr);
		    if (t != 0)
		    {
		      (*(pte_t *)t) &= read_write_mask;
#ifdef SIM			
		      tlb_sync(&vas[x->task].tcb, x->vaddr, (*(pte_t *)t));
#else
		      tlb_sync((tcb_t *)
			       (((udw_t)(x->task)
				 << 36 >> 46 <<  TCB_SIZE_SHIFT)
				+ TCB_VBASE), x->vaddr,(*(pte_t *)t));
		      
#endif			
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
	      
	      if ((dw_t)mask < 0)
	      {
		if (mask & L4_FPAGE_RW_MASK)
		{
		  *pte = 0;
#ifdef SIM		    
		  tlb_sync(&vas[start->task].tcb, vaddr, 0);
#else
		  tlb_sync(tcb, vaddr, 0);
#endif
		  
		  /* remove start as well */
		  if ( ((mt_node_t *)((mt_node_t *)start->parent)->child)
		       == start)
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
		  *pte &= read_write_mask;
#ifdef SIM		    
		  tlb_sync(&vas[start->task].tcb, vaddr, *pte);
#else
		  tlb_sync(tcb, vaddr, *pte);
#endif		    
		}
	      }
	    }
	  }
	  i++;
	}
	else
	{
	  push_pos(gpt,i+1,level,size,&sp);
	  level = gptr->guard & 077;
	  gpt = (gpt_t *)(gptr->u.ptr - 15);
	  i = 0;
	  size = GPTSIZE;
	}
      }
    } 
  } while (vaddr < end && level != WORDLEN);
}


void gpt_free_tree(gpt_t *gpt)
{
  ub_t level, size;
  unsigned short i;
  void *sp;

  gpt = (gpt_t *) ((udw_t) gpt - 15);
  sp = alloca((WORDLEN/GPTSIZE+1)*8);
  size = GPTROOTSIZE;
  push_pos(gpt,0,WORDLEN,size,&sp);
  do
  {
    pop_pos(&gpt,&i,&level,&size,&sp);
    while  (i < (1 << size))
    {
      if (gpt[i].guard == NULL_GUARD(((udw_t)level),
				     ((udw_t)size),
				     ((udw_t)i)))
      {
	i++;
      }
      else
      {
	if (
	    
	    (
	     (~(udw_t)i & ((1ul << size) - 1ul)) 
	     == 
	     (
	      (gpt[i].guard >> (level - size)) & 
	      ((1ul << size) - 1ul)
	      )
	     ) ||
	    (
	     ((1ul << size) + i)
	     ==
	     ((gpt[i].guard >> (level - size))
	      & ((1ul << (size + 1)) - 1ul))
	     )
	    
	    )
	{
	  i++;
	}
	else
	{
	  push_pos(gpt,i+1,level,size,&sp);
	  size = GPTSIZE;
	  level = gpt[i].guard & 077;
	  gpt = (gpt_t *)(gpt[i].u.ptr - 15);
	  i = 0;
	}
      }
    }
    gpt_mem_free(gpt);
  } while (level != WORDLEN);
}
