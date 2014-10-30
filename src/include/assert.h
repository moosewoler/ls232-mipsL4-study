#ifndef _ASSERT_H
#define _ASSERT_H
/****************************************************************************
 * $Id: assert.h,v 1.3 1998/01/22 05:45:50 kevine Exp $
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
#ifdef NDEBUG
#undef assert
#define assert(EX) ((void)0)

#else

extern void __assert(const char *, const char *, int);
#if defined( __ANSI_CPP__) || defined(__GNUC__)
#define assert(EX)  ((EX)?((void)0):__assert(#EX , __FILE__, __LINE__))
#else
#define assert(EX)  ((EX)?((void)0):__assert("EX", __FILE__, __LINE__))
#endif
#endif /* NDEBUG */
#endif
