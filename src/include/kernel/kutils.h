#ifndef KUTILS_H
#define KUTILS_H
/****************************************************************************
 * $Id: kutils.h,v 1.4 1998/01/22 05:46:09 kevine Exp $
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

#ifdef _LANGUAGE_C

extern int sprintf (char *, const char *, ...);
extern void wbflush(void);
extern void kprintf(char *, ...);
extern void ints_on(void);
extern void ints_off(void);

/* string handling stuff */
extern int strcmp(const char *s1, const char *s2);

/* ctype stuff */
extern int isspace (int c);
extern int iscntrl (int c);

/* misc funcs */
extern int llatob (unsigned long *vp, char *p, int base);
#endif

#endif
