#ifndef DIT_H
#define DIT_H
/****************************************************************************
 * $Id: dit.h,v 1.4 1998/01/22 05:46:03 kevine Exp $
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

#define DIT_NIDENT  4
#define DIT_NPNAME 16

#define DHDR_SEG_SIZE 4096
#define DHDR_ALIGN 4096

#define DIT_RUN         1


#if defined(_LANGUAGE_ASSEMBLY)

#define D_D_IDENT       0
#define D_D_PHOFF       4
#define D_D_PHSIZE      8
#define D_D_PHNUM      12
#define D_D_FILEEND    16
#define D_D_VADDREND   20

#define D_P_BASE        0
#define D_P_SIZE        4
#define D_P_ENTRY       8
#define D_P_FLAGS      12
#define D_P_NAME       16

#else /* assume C */

typedef unsigned int Dit_uint;

typedef struct {
  unsigned char d_ident[DIT_NIDENT];
  Dit_uint d_phoff;
  Dit_uint d_phsize;
  Dit_uint d_phnum;
  Dit_uint d_fileend;
  Dit_uint d_vaddrend;
} Dit_Dhdr;

typedef struct {
  Dit_uint p_base;
  Dit_uint p_size;
  Dit_uint p_entry;
  Dit_uint p_flags;
  unsigned char p_name[DIT_NPNAME];
} Dit_Phdr;

#endif
#endif
