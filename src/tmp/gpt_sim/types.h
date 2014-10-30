#ifndef TYPES_H
#define TYPES_H
/****************************************************************************
 *      $Id: types.h,v 1.2 1998/01/22 05:49:52 kevine Exp $
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

#ifdef _LANGUAGE_C
/* using mips terminology */

/* 64 bit ints (double word) */
typedef unsigned long udw_t;
typedef long dw_t;

/* 32-bit ints (word) */
typedef unsigned int uw_t;
typedef int w_t;

/* 16-bit ints (half word) */
typedef unsigned short int uhw_t;
typedef short int hw_t;

/* 8-bit ints (bytes) */
typedef unsigned char ub_t;
typedef char b_t;


#endif
#endif
