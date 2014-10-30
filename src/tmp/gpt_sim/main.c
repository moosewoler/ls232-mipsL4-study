/****************************************************************************
 *      $Id: main.c,v 1.12 1998/03/19 23:41:27 kevine Exp $
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

#undef MAIN_DEBUG
#undef AC_DEBUG
#undef UN_DEBUG
#define TESTSIZE 24

#include <kernel/vm.h>
#include <kernel/kmem.h>
#include <r4kc0.h>
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "debug.h"
#include "sim.h"

#define FRAME_USED 1ul
extern dword_t randnum(void);
tcb_t sigma0_tcb;
extern dword_t *frame_table_base;
dword_t mt_mem_count;
dword_t gpt_mem_count;


sim_as_t vas[MAXAS];
dword_t  *tlb2_base_ptr;
dword_t randcount;
dword_t c0_ehi;
dword_t c0_bva;
word_t c0_count;

kernel_vars sim_kern;

w_t build_as(w_t *as_number, w_t pager, w_t depth)
{
  w_t i, my_node_num, r;
  
  r = NILAS;
  if (*as_number == MAXAS || depth == 0)
  {
    return r;
  }
  
  /* create this node */
#ifdef AS_DEBUG
  printf("Creating AS %d pager %d\n", *as_number, pager);
#endif
  vas[*as_number].pager = pager;
  vas[*as_number].tcb.myself = *as_number;
  vas[*as_number].tcb.asid = *as_number;
  if (*as_number != 0)
  {
    vm_new_as(&vas[*as_number].tcb);
  }
  if (vas[*as_number].tcb.gpt_pointer == 0)
  {
    abort();
  }
  for (i = 0; i < MAXPG; i++)
  {
    vas[*as_number].v_table[i] = 0;
  }
  r = my_node_num = *as_number;
  
  (*as_number)++;

  /* create clients */

  for (i = 0; i < FAN_OUT; i++)
  {
    vas[my_node_num].child[i] =
      build_as(as_number, my_node_num, depth - 1);
  }
  return r;
}

void delete_as(w_t as)
{
  int i;
  if (as == NILAS) return;
  
  for (i = 0; i < FAN_OUT; i++)
  {
    delete_as(vas[as].child[i]);
  }
  if (as != 0)
  {
    
    vm_fpage_unmap(&vas[as].tcb, 64 << 2, (1ul << 63) | (1ul << 1));
    vm_delete_as(&vas[as].tcb);
  }
}

#define v2ft(v) ((udw_t *)((udw_t) frame_table_base |((udw_t) v >> (FRAME_SIZE_SHIFT -3))))

w_t do_access(w_t as, udw_t vaddr, uw_t perm)
{
  dw_t r;
  udw_t v;
  pte_t *leaf;
#ifdef AC_DEBUG
  printf("do_access: attempting as: %d v: 0x%x p: %d\n",
	 as, vaddr, perm);
#endif
  if (as == NILAS)
  {
    return 0;
  }
  else if (as == 0)
  {
    for (v = vaddr; v < (vaddr + (1ul << TESTSIZE)); v += FRAMESIZE)
    {
      leaf = vm_lookup_pte(sigma0_tcb.gpt_pointer, v);
      
      if (leaf == 0 || (!(*leaf & EL_Valid)))
      {
	vm_sigz_insert_pte(v, EL_Valid | (1 << 2));
	leaf = vm_lookup_pte(sigma0_tcb.gpt_pointer, v);
	if (leaf == 0)
	{
	  abort();
	}
#ifdef  AC_DEBUG
	printf("inserting in sigma0\n");
#endif
      }
      vas[as].v_table[v >> FRAME_SIZE_SHIFT] = EL_Valid | (1 << 2);
    }
    return 0;
  }
  else
  {
    w_t i;
    dw_t valid;
    
    /* first check validity of address space */
    for(i = 0; i < MAXPG; i++)
    {
      leaf = vm_lookup_pte(vas[as].tcb.gpt_pointer, i << FRAME_SIZE_SHIFT);

      if (leaf == 0 && vas[as].v_table[i] != 0)
      {
	abort();
      }
      if (leaf != 0 &&
	  (*leaf & (EL_Valid | EL_Dirty)) !=
	  (vas[as].v_table[i] & (EL_Valid | EL_Dirty)))
      {
	abort();
      }
    }
    
    /* handle normal as */
    
    leaf = vm_lookup_pte(vas[as].tcb.gpt_pointer, vaddr);
    
    if (perm)
    {
      valid = (leaf != 0) &&
	((*leaf & (EL_Valid|EL_Dirty)) == (EL_Valid|EL_Dirty));
    }
    else
    {
      valid = (leaf != 0) &&
	((*leaf & (EL_Valid)) == (EL_Valid));
    }
    
    if (valid)
    {
#ifdef AC_DEBUG
      printf("access as %d vaddr 0x%lx\n", as, vaddr);
#endif
      return 0;
    }
    
    r = do_access(vas[as].pager, vaddr, perm);
    if (r == -1)
    {
#ifdef AC_DEBUG
      printf("failed access as %d vaddr 0x%lx\n", as, vaddr);
#endif
      return -1;
    }
    
    if (vas[as].pager == NILAS)
    {
      vm_map(&sigma0_tcb,
	     vaddr | (TESTSIZE << 2) | (perm << 1),
	     vaddr,
	     &vas[as].tcb,
	     64 << 2);
    }
    else
    {
      vm_map(&vas[vas[as].pager].tcb,
	     vaddr | (TESTSIZE << 2) | (perm << 1),
	     vaddr,
	     &vas[as].tcb,
	     64 << 2);
    }
    leaf = vm_lookup_pte(vas[as].tcb.gpt_pointer, vaddr);
    if (leaf == 0)
    {
      abort();
    }
    
    for (v = vaddr; v < (vaddr + (1ul << TESTSIZE)); v += FRAMESIZE)
    {
      
      vas[as].v_table[v >> FRAME_SIZE_SHIFT] = EL_Valid | (perm << 2);
    }
    /* first check validity of address space */
    for(i = 0; i < MAXPG; i++)
    {
      leaf = vm_lookup_pte(vas[as].tcb.gpt_pointer, i << FRAME_SIZE_SHIFT);
      
      if (leaf == 0 && vas[as].v_table[i] != 0)
      {
	abort();
      }
      if (leaf != 0 &&
	  (*leaf & (EL_Valid | EL_Dirty)) !=
	  (vas[as].v_table[i] & (EL_Valid | EL_Dirty)))
      {
	abort();
      }
    }
    
    
#ifdef AC_DEBUG
    printf("map access as %d vaddr 0x%lx\n", as, vaddr);
#endif
    return 0;
  }
}
void do_unmap(udw_t vaddr, w_t as, w_t depth, uw_t perm)
{
  udw_t fpage;
  pte_t *leaf;
  dw_t mask;
  w_t i;
  

  if (as == NILAS)
    return;
#ifdef UN_DEBUG
  printf("do_unmap: vaddr 0x%lx as %d\n", vaddr, as);
#endif
  
  if (depth == 0)
  {
    /* first do the syscall */
    fpage = vaddr | (TESTSIZE << 2);
    if (as != 0)
    {
      mask = (dw_t) (1ul << 63) | (perm << 1);
    }
    else
    {
      mask = (perm << 1);
    }
    vm_fpage_unmap(&vas[as].tcb, fpage, mask);
  }

  /* now check the data structures */
  if (as != 0)
  {
    udw_t v;
    for (v = vaddr; v < (vaddr + (1ul << TESTSIZE)); v += FRAMESIZE)
    {
      leaf =  vm_lookup_pte(vas[as].tcb.gpt_pointer, v);
      if (perm)
      {
#ifdef UN_DEBUG
	printf("do_unmap: invalidating\n");
#endif
	vas[as].v_table[v >> FRAME_SIZE_SHIFT] = 0;
      }
      else
      {
	vas[as].v_table[v >> FRAME_SIZE_SHIFT] &= ~EL_Dirty;
#ifdef UN_DEBUG            
	printf("do_unmap: making ro\n");
#endif
      }
      if (leaf == 0 && vas[as].v_table[v >> FRAME_SIZE_SHIFT] != 0)
      {
	abort();
      }
      if (leaf != 0 &&
	  (*leaf & (EL_Valid | EL_Dirty)) !=
	  (vas[as].v_table[v >> FRAME_SIZE_SHIFT] & (EL_Valid | EL_Dirty)))
      {
	abort();
      }
    }
  }
  
  /* now check the children have been unmapped */
  for (i = 0; i < FAN_OUT; i++)
  {
    do_unmap(vaddr, vas[as].child[i], 1, perm);
  }
}

dword_t tcb_sizeof;

void main()
{
    dw_t r;
    udw_t i;
    w_t as_number, perm;
    uw_t lo0, lo1;
    pte_t *leaf;
      udw_t addr;

    tcb_sizeof = sizeof(sim_as_t);

    sigma0_init((void *)0xffffffff80008000, MEGRAM * 1024*1024);
    vm_mem_init();
    tlb2_base_ptr = memalign(128*1024,128*1024);
#ifdef	TLB_CACHE
    assert(tlb2_base_ptr != 0);
    tlb_cache_init();
#endif

/****************************************************************************
 * create the initial address space
 ****************************************************************************/
    
    vm_initial_as();
    sim_kern.sigz_tcb = &sigma0_tcb;
    sigma0_tcb.gpt_pointer = vas[0].tcb.gpt_pointer;
    as_number = 0;

/****************************************************************************
 * build up a hierachy of tasks, each with it's own address space
 ****************************************************************************/    
    r = build_as(&as_number,NILAS,PAGER_DEPTH);

    if (sigma0_tcb.gpt_pointer)
    {
      printf("Alloc Okay\n");
    }
    else
    {
	printf("Gpt failed\n");
	abort();
    }

/****************************************************************************
 * Randomly create some mappings in tasks, propagating page faults back to
 * sigma0 if appropriate
 ****************************************************************************/

    for (i = 0; i < 10000; i++)
    {

      do {
	addr = randnum()  &  MEGRAM*1024*1024 -1 &(~((1ul << TESTSIZE) -1ul));
      } while  (addr < (RESERVED_RAM * 1024 *1024));
      
      as_number = (w_t) (randnum() & (MAXAS - 1));
      perm = (w_t) (randnum() & 1);
#ifdef MAIN_DEBUG      
      printf("mt_mem %ld gpt_mem %ld\n", mt_mem_count, gpt_mem_count);
#endif       
      randcount++;
      r = do_access(as_number, addr, perm);
      vm_check_map_trees((void *) addr);
      leaf = vm_lookup_pte(vas[as_number].tcb.gpt_pointer, addr);
      if (leaf != 0)
      {
#ifdef MAIN_DEBUG  
	printf("v:0x%x pte:0x%x\n", addr, *leaf);
#endif
      }


      if (addr & FRAMESIZE)
      {
	
	leaf = vm_lookup_pte(vas[as_number].tcb.gpt_pointer, addr-4096);
	if (leaf != 0)
	{
#ifdef MAIN_DEBUG  
	  printf("v:0x%x pte:0x%x\n", addr-4096, *leaf);
#endif
	}
	else
	{
#ifdef MAIN_DEBUG  
	   printf("v:0x%x pte: null\n", addr-4096);
#endif
	}
      }
      else
      {
	
	leaf = vm_lookup_pte(vas[as_number].tcb.gpt_pointer, addr+4096);
	if (leaf != 0)
	{
#ifdef MAIN_DEBUG  
	  printf("v:0x%x pte:0x%x\n", addr+4096, *leaf);
#endif
	}
	else
	{
#ifdef MAIN_DEBUG  
	  printf("v:0x%x pte: null\n", addr+4096);
#endif
	}
      }      
      c0_count ++;
      c0_ehi = (addr & ~8191ul) | as_number;
      c0_bva = addr;
      sim_kern.gpt_pointer = vas[as_number].tcb.gpt_pointer;
      r = xtlb_refill(&lo0,&lo1);
      if (r)
      {
#ifdef MAIN_DEBUG 
	printf("access main tlb2 HIT: v:0x%x lo0:%x lo1:%x\n", addr, lo0, lo1);
#endif
      }
      else
      {
#ifdef MAIN_DEBUG 
	printf("access main tlb2 MISS: v:0x%x lo0:%x lo1:%x\n", addr, lo0, lo1);
#endif
      }
      /* gpt_lookup_pair(vas[as_number].gpt, addr, &lo0, &lo1);*/

      if (addr & FRAMESIZE)
      {
	if (((lo1 & (EL_Valid | EL_Dirty)) !=
	     (vas[as_number].v_table[addr >> FRAME_SIZE_SHIFT]
	      & (EL_Valid | EL_Dirty))))
	{
	  abort();
	}
	if (lo0 != 0)
	{
	  if (((lo0 & (EL_Valid | EL_Dirty)) !=
	       (vas[as_number].v_table[(addr >> FRAME_SIZE_SHIFT)-1]
		& (EL_Valid | EL_Dirty))))
	  {
	    abort();
	  }
	}
      }
      else
      {
	if (((lo0 & (EL_Valid | EL_Dirty)) !=
	     (vas[as_number].v_table[addr >> FRAME_SIZE_SHIFT]
	      & (EL_Valid | EL_Dirty))))
	{
	  abort();
	}
	if (lo1 != 0)
	{
	  if (((lo1 & (EL_Valid | EL_Dirty)) !=
	       (vas[as_number].v_table[(addr >> FRAME_SIZE_SHIFT)+1]
		& (EL_Valid | EL_Dirty))))
	  {
	    abort();
	  }
	}
      }
    }
    
    printf("gpt address spaces created\n");
    fflush(stdout);

    /* probe the entire address space */

    for (as_number = 0; as_number <  MAXAS; as_number++)
    {
#ifdef MAIN_DEBUG 
      printf("checking address space %d\n",
	     as_number);
#endif
      for (addr = RESERVED_RAM * 1024 *1024;
	   addr < MEGRAM *1024*1024;
	   addr += FRAMESIZE)
      {
	c0_count ++;
	c0_ehi = (addr & ~8191ul) | as_number;
	c0_bva = addr;
	sim_kern.gpt_pointer = vas[as_number].tcb.gpt_pointer;
	r = xtlb_refill(&lo0,&lo1);
	if (r)
	{
#ifdef ADDR_DEBUG	  
	  printf("access main tlb2 HIT: v:0x%x lo0:%x lo1:%x\n",
		 addr, lo0, lo1);
#endif
	}
	else
	{
#ifdef ADDR_DEBUG 
	  printf("access main tlb2 MISS: v:0x%x lo0:%x lo1:%x\n",
		 addr, lo0, lo1);
#endif
	}
	/* gpt_lookup_pair(vas[as_number].gpt, addr, &lo0, &lo1);*/
	
	if (addr & FRAMESIZE)
	{
	  if (((lo1 & (EL_Valid | EL_Dirty)) !=
	       (vas[as_number].v_table[addr >> FRAME_SIZE_SHIFT]
		& (EL_Valid | EL_Dirty))))
	  {
	    abort();
	  }
	  if (lo0 != 0)
	  {
	    if (((lo0 & (EL_Valid | EL_Dirty)) !=
		 (vas[as_number].v_table[(addr >> FRAME_SIZE_SHIFT)-1]
		  & (EL_Valid | EL_Dirty))))
	    {
	      abort();
	    }
	  }
	}
	else
	{
	  if (((lo0 & (EL_Valid | EL_Dirty)) !=
	       (vas[as_number].v_table[addr >> FRAME_SIZE_SHIFT]
		& (EL_Valid | EL_Dirty))))
	  {
	    abort();
	  }
	  if (lo1 != 0)
	  {
	    if (((lo1 & (EL_Valid | EL_Dirty)) !=
		 (vas[as_number].v_table[(addr >> FRAME_SIZE_SHIFT)+1]
		  & (EL_Valid | EL_Dirty))))
	    {
	      abort();
	    }
	  }
	}
      }
    }
    
/****************************************************************************
 * randomly unmap pages in tasks checking if child tasks are also unmapped
 ****************************************************************************/

    for (i = 0; i < 100000; i++)
    {
      udw_t addr;

      do {
      addr = randnum()  &  MEGRAM *1024*1024 -1 &(~((1ul << TESTSIZE) -1ul));
      } while  (addr < (RESERVED_RAM * 1024 *1024));

      as_number = (w_t)(randnum() & (MAXAS - 1));
#ifdef MAIN_DEBUG 
      printf("mt_mem %ld gpt_mem %ld\n", mt_mem_count, gpt_mem_count);
#endif
      perm = (w_t) (randnum() & 1);
      do_unmap(addr, as_number, 0, perm);
      randcount++;

      leaf = vm_lookup_pte(vas[as_number].tcb.gpt_pointer, addr);
      if (leaf != 0)
      {
#ifdef MAIN_DEBUG 
	printf("v:0x%x pte:0x%x\n", addr, *leaf);
#endif
      }

      if (addr & FRAMESIZE)
      {
	
	leaf = vm_lookup_pte(vas[as_number].tcb.gpt_pointer, addr-4096);
	if (leaf != 0)
	{
#ifdef MAIN_DEBUG 
	  printf("v:0x%x pte:0x%x\n", addr-4096, *leaf);
#endif
	}
	else
	{
#ifdef MAIN_DEBUG 
	  printf("v:0x%x pte: null\n", addr-4096);
#endif
	}
      }
      else
      {
	
	leaf = vm_lookup_pte(vas[as_number].tcb.gpt_pointer, addr+4096);
	if (leaf != 0)
	{
#ifdef MAIN_DEBUG 
	  printf("v:0x%x pte:0x%x\n", addr+4096, *leaf);
#endif
	}
	else
	{
#ifdef MAIN_DEBUG 
	  printf("v:0x%x pte: null\n", addr+4096);
#endif
	}
      }      
      
      /* gpt_lookup_pair(vas[as_number].tcb.gpt_pointer, addr, &lo0, &lo1); */
      
      c0_count ++;
      c0_ehi = (addr & ~8191ul) | as_number;
      c0_bva = addr;
      sim_kern.gpt_pointer = vas[as_number].tcb.gpt_pointer;
      r = xtlb_refill(&lo0,&lo1);

      if (r)
      {
#ifdef MAIN_DEBUG 
	printf("unmap main tlb2 HIT: v:0x%x lo0:%x lo1:%x\n", addr, lo0, lo1);
#endif
      }
      else
      {
#ifdef MAIN_DEBUG 
	printf("unmap main tlb2 MISS: v:0x%x lo0:%x lo1:%x\n", addr, lo0, lo1);
#endif
      }

      if (addr & FRAMESIZE)
      {
	if (((lo1 & (EL_Valid | EL_Dirty)) !=
	(vas[as_number].v_table[addr >> FRAME_SIZE_SHIFT]
	 & (EL_Valid | EL_Dirty))))
	{
	  abort();
	}
	if (lo0 != 0)
	{
	  if (((lo0 & (EL_Valid | EL_Dirty)) !=
	       (vas[as_number].v_table[(addr >> FRAME_SIZE_SHIFT)-1]
		& (EL_Valid | EL_Dirty))))
	  {
	    abort();
	  }
	}
      }
      else
      {
	if (((lo0 & (EL_Valid | EL_Dirty)) !=
	(vas[as_number].v_table[addr >> FRAME_SIZE_SHIFT]
	 & (EL_Valid | EL_Dirty))))
	{
	  abort();
	}
	if (lo1 != 0)
	{
	  if (((lo1 & (EL_Valid | EL_Dirty)) !=
	       (vas[as_number].v_table[(addr >> FRAME_SIZE_SHIFT)+1]
		& (EL_Valid | EL_Dirty))))
	  {
	    abort();
	  }
	}
      }
    }

    printf("unmaping worked\n");

    
/****************************************************************************
 * test the delete as code, it first unmaps address space and them
 * free's tree.
 ****************************************************************************/
    delete_as(0);
    printf("Address spaces deleted\n");
    vm_tcb_insert(vas[0].tcb.gpt_pointer,
		       0xc000000000001000ul, 0x1000ul);
    vm_tcb_insert(vas[0].tcb.gpt_pointer,
		       0xc000000000004000ul, 0x2000ul);
    vm_tcb_insert(vas[0].tcb.gpt_pointer,
		       0xc000000000008000ul, 0x4000ul);
    vm_tcb_insert(vas[0].tcb.gpt_pointer,
		       0xc000000000010000ul, 0x3000ul);
    vm_tcb_insert(vas[0].tcb.gpt_pointer,
		       0xc000000000014000ul, 0x5000ul);
    vm_tcb_insert(vas[0].tcb.gpt_pointer,
		       0xc000000000018000ul, 0x6000ul);
    
    vm_print_fpage(vas[0].tcb.gpt_pointer,0,64); 
    vm_tcb_unmap(vas[0].tcb.gpt_pointer,0xc000000000000000ul);
    vm_print_fpage(vas[0].tcb.gpt_pointer,0,64);
    
}




