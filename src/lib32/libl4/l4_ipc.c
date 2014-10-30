/****************************************************************************
 *      $Id: l4_ipc.c,v 1.3 1999/03/08 07:35:23 gernot Exp $
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

#include <l4/ipc.h>

const l4_timeout_t _l4_ipc_never = {0ul};
const l4_threadid_t _l4_sigma0_tid = {(1 << 17)};

int l4_ipc_fpage_received(l4_msgdope_t msgdope)
{
  return msgdope.md.fpage_received != 0;
}

int l4_ipc_is_fpage_granted(l4_fpage_t fp)
{
  return fp.fp.grant != 0;
}

int l4_ipc_is_fpage_writable(l4_fpage_t fp)
{
  return fp.fp.write != 0;
}

