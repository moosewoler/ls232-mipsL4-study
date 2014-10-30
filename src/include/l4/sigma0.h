#ifndef __L4_SIGMA0_H__
#define __L4_SIGMA0_H__
/****************************************************************************
 * $Id: sigma0.h,v 1.4 1998/12/23 02:03:59 gernot Exp $
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

#include <l4/types.h>

/****************************************************************************
 * define some constants relevent to sigma0
 ****************************************************************************/

#ifdef _LANGUAGE_C
#ifdef __GNUC__
#define SIGMA0_DEV_MAP           (0xfffffffffffffffeULL)
#define SIGMA0_KERNEL_INFO_MAP   (0xfffffffffffffffdULL)
#define SIGMA0_TID               ((l4_threadid_t) {ID: (1 << 17)})
#else
#define SIGMA0_DEV_MAP           (0xfffffffffffffffeul)
#define SIGMA0_KERNEL_INFO_MAP   (0xfffffffffffffffdul)
#define SIGMA0_TID           _l4_sigma0_tid
extern const l4_threadid_t _l4_sigma0_tid;
#endif
#else
#define SIGMA0_DEV_MAP           0xfffffffffffffffe
#define SIGMA0_KERNEL_INFO_MAP   0xfffffffffffffffd
#define SIGMA0_TID               (1 << 17)
#endif

/****************************************************************************
 * define format of kernel info page
 ****************************************************************************/

#ifdef _LANGUAGE_C
typedef struct {
  word_t magic; /* L4uK */
  hword_t version;
  hword_t build;
  dword_t clock;
  dword_t memory_size;
  dword_t kernel;
  dword_t dit_hdr;
  dword_t kernel_data;
} l4_kernel_info;
#else
#define LKI_MAGIC           0
#define LKI_VERSION         4
#define LKI_BUILD           6
#define LKI_CLOCK           8
#define LKI_MEMORY_SIZE    16
#define LKI_KERNEL         24
#define LKI_DIT_HDR        32
#define LKI_KERNEL_DATA    40

#endif
#endif


