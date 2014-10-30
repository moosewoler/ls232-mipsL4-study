/****************************************************************************
 *      $Id: macros.h,v 1.1 1998/12/29 05:23:41 gernot Exp $
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


/****************************************************************************
 *
 * stack_c & unstack_c
 *
 * to preserve C calling conventions we need to preserve
 * and restore s0-7 registers and gp, fp, ra for system call 
 *
 ****************************************************************************/

#define stack_c \
	sd	ra, (sp) ; \
	sd	fp, 8(sp) ; \
	sd	gp, 16(sp) ; \
	sd	s7, 24(sp) ; \
	sd	s6, 32(sp) ; \
	sd	s5, 40(sp) ; \
	sd	s4, 48(sp) ; \
	sd	s3, 56(sp) ; \
	sd	s2, 64(sp) ; \
	sd	s1, 72(sp) ; \
	sd	s0, 80(sp)
	

#define unstack_c \
	ld	ra, (sp) ; \
	ld	fp, 8(sp) ; \
	ld	gp, 16(sp) ; \
	ld	s7, 24(sp) ; \
	ld	s6, 32(sp) ; \
	ld	s5, 40(sp) ; \
	ld	s4, 48(sp) ; \
	ld	s3, 56(sp) ; \
	ld	s2, 64(sp) ; \
	ld	s1, 72(sp) ; \
	ld	s0, 80(sp)
	
/****************************************************************************
 *
 * msg_to_reg(reg) and reg_to_msg
 *
 * copy registers to and from a C buffer
 *
 ****************************************************************************/

#define msg_to_reg(reg) \
	ld	s0, (reg) ; \
	ld	s1, 8(reg) ; \
	ld	s2, 16(reg) ; \
	ld	s3, 24(reg) ; \
	ld	s4, 32(reg) ; \
	ld	s5, 40(reg) ; \
	ld	s6, 48(reg) ; \
	ld	s7, 56(reg)

#define reg_to_msg(reg) \
	sd	s0, (reg) ; \
	sd	s1, 8(reg) ; \
	sd	s2, 16(reg) ; \
	sd	s3, 24(reg) ; \
	sd	s4, 32(reg) ; \
	sd	s5, 40(reg) ; \
	sd	s6, 48(reg) ; \
	sd	s7, 56(reg)
