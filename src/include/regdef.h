#ifndef REGDEF_H
#define REGDEF_H
/****************************************************************************
 * $Id: regdef.h,v 1.3 1998/01/22 05:45:53 kevine Exp $
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

/****************************************************************************
 *
 * General register names (64 bit) sourced from MIPSpro Assembly Language
 * Programmers guide.
 *
 ****************************************************************************/

#define zero $0
#define AT   $1
#define v0   $2
#define v1   $3
#define a0   $4
#define a1   $5
#define a2   $6
#define a3   $7
#define a4   $8

#if _MIPS_SZPTR == 64

/* assume 64-bit sgi compiler */
#define ta0  $8
#define a5   $9
#define ta1  $9
#define a6  $10
#define ta2 $10
#define a7  $11
#define ta3 $11
#define t0  $12
#define t1  $13
#define t2  $14
#define t3  $15

#else

/* gcc 32-bit compiler */
#define t0   $8
#define t1   $9
#define t2  $10
#define t3  $11
#define t4  $12
#define t5  $13
#define t6  $14
#define t7  $15

#endif

#define s0  $16
#define s1  $17
#define s2  $18
#define s3  $19
#define s4  $20
#define s5  $21
#define s6  $22
#define s7  $23
#define t8  $24
#define t9  $25
#define k0  $26
#define k1  $27
#define gp  $28
#define sp  $29
#define fp  $30
#define s8  $30
#define ra  $31


#endif
