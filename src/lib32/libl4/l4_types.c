/****************************************************************************
 *      $Id: l4_types.c,v 1.3 1999/03/08 07:35:24 gernot Exp $
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

#include <l4/types.h>

l4_fpage_t l4_fpage(dword_t address, unsigned int size, 
				  unsigned char write, unsigned char grant)
{
  l4_fpage_t r;
  /* return ((l4_fpage_t){fp:{grant, write, size, 0, 
			     (address & L4_PAGEMASK) >> 12,0 }});*/
  r.fpage = grant | (write << 1) | (size << 2) | (address & L4_PAGEMASK);
  return r;
  
}

l4_threadid_t get_taskid(l4_threadid_t t)
{
  t.id.lthread = 0;
  return t; 
}

extern int task_equal(l4_threadid_t t1,l4_threadid_t t2)
{
  t1.id.lthread = 0;
  t2.id.lthread = 0;
  return (t1.ID == t2.ID);
}

l4_timeout_t L4_IPC_TIMEOUT(byte_t snd_man, byte_t snd_exp,
			    byte_t rcv_man, byte_t rcv_exp,
			    byte_t snd_pflt, byte_t rcv_pflt)
{
  l4_timeout_t t;
  t.timeout =   rcv_man << 24 |  snd_man << 16 | rcv_pflt << 12 |
    snd_pflt << 8 | snd_exp << 4 | rcv_exp ;
  return t;
}
