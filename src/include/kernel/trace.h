#ifndef TRACE_H
#define TRACE_H
/****************************************************************************
 * $Id: trace.h,v 1.3 1998/01/22 05:46:12 kevine Exp $
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

#define TRACING 1

#define T_SIGMA0_INIT 0 /* trace sigma0_init */
#define T_SIGMA0_PT 0   /* trace sigma0_pt_insert */
#define T_GPT_MAP 0     /* trace gpt map */
#define T_GPT_UMAP 0    /* trace gpt unmap */
#define T_PROCS 1

#ifdef TRACING

extern void alphn_set(const char *);

#define TCODE(flag, code) if (flag) { code  }
#define LABEL(lab) static const char label[] = lab
#define TRACE(flag) if (flag) alphn_set(label)
#else

#define TCODE(flag, code)
#define LABEL(lab)
#define TRACE(flag)

#endif

#endif 





