/****************************************************************************
 *      $Id: machdep.c,v 1.4 1998/01/22 05:49:47 kevine Exp $
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
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

const l4_threadid_t _l4_sigma0_tid = {(1 << 17)};

udw_t randnum(void)
{

#if defined(sparc)
    
    return lrand48();
    
#elif defined(linux) || defined(__alpha) || defined(__sgi)
    
    return random();
    
#endif
}

void srandnum(udw_t s)
{
    
#if defined(sparc)
    
    srand48(s);
    
#elif defined(linux) || defined(__alpha) || defined (__sgi)
    
    srandom((unsigned int)s);
    
#endif
}

char outbuff[1024];

void printbuff()
{
  printf("%s",outbuff);
}

void tlb_sync_shared(dword_t vaddr)
{
}

void tlb_sync(tcb_t *tcb, dword_t vaddr, pte_t pte)
{
}
