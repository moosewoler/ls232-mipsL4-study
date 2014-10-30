#ifndef ASM_H
#define ASM_H
/****************************************************************************
 * $Id: asm.h,v 1.2 1998/01/22 05:45:49 kevine Exp $
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

#define LEAF(x) \
	.text			; \
	.ent x			; \
	.globl x		; \
x:


#define PROC(x) \
	.text ;\
	.ent x ;\
	.globl x ;\
x:
       
	
#define END(x) \
	.end x

#define IMPORT(name, size) \
        .extern name,size

#endif
