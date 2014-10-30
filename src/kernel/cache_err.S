/****************************************************************************
 *      $Id: cache_err.S,v 1.2 1999/03/22 06:26:27 gernot Exp $
 *      Copyright (C) 1997, 1998 Kevin Elphinstone, Univeristy of New South
 *      Wales.
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
#include <regdef.h>
#include <kernel/machine.h>
#include <kernel/kernel.h>
#include <r4kc0.h>
#include <asm.h>
#include <l4/types.h>

/****************************************************************************
 *
 * Cache Error Handling Routines
 *
 ****************************************************************************
 *
 * cache_err_init: Load the cache_err routine into the right place to get
 * called
 *
 * cache_err: Simply print "cerr" on the alphn display and "lock up"
 *
 ****************************************************************************/
	
PROC(cache_err_init)
	dla	a0, cache_err_start
	dla     a3, cache_err_end
	dli	a1, CACHE_ERR_VEC
1:
	lw	a2, (a0)
	addiu	a0, a0, 4
	sw	a2, (a1)
	addiu   a1, a1, 4
	bne     a0, a3, 1b
	jr	ra
END(cache_err_init)
	.data
cache_err_msg:
	.asciiz "L4 PANIC: cache error exception"
PROC(cache_err)
	.set noreorder
	.set noat
cache_err_start:
#ifdef P4000
	dli	a1, PHYS_TO_CKSEG1(ALPHN_CHAR_BASE)
	li	a2, 'c'
	sll	a2, 24
	sw	a2, 12(a1)
	li	a2, 'e'
	sll	a2, 24
	sw	a2, 8(a1)
	li	a2, 'r'
	sll	a2, 24
	sw	a2, 4(a1)
	li	a2, 'r'
	sll	a2, 24
	sw	a2, (a1)
	
1:	nop
#endif
	dla	AT, panic /* ok */
	dla	a0, cache_err_msg
	jr	AT
	nop
cache_err_end:
	.set at
	.set reorder
END(cache_err)

PROC(dcache_hit_wb)
	/* write back data cache line, at a0 */
	.set noreorder
	cache	D_HIT_WB, 0(a0)
	jr	ra
	nop
	.set reorder
END(dcache_hit_wb)

PROC(dcache_page_invalidate)
	.set	noreorder
	dli	t1, ~(L4_PAGESIZE-1)
	and	a0, a0, t1
	daddiu	t0, a0, L4_PAGESIZE - 128
	
1:	cache	D_HIT_INV, 0(t0)
	cache	D_HIT_INV, 32(t0)		
	cache	D_HIT_INV, 64(t0)
	cache	D_HIT_INV, 96(t0)
	bne	t0, a0, 1b
	daddiu	t0, t0, -128

	jr	ra
	nop
	.set	reorder
END(dcache_page_invalidate)
