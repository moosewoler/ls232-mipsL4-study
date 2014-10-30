/****************************************************************************
 *      $Id: utils.c,v 1.2 1998/01/22 05:50:38 kevine Exp $
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
#include "protos.h"

int little_endian;

void set_endian(unsigned int x)
{
  switch (x)
  {
    /* 0x7f 'E' 'L' 'F' */
  case 0x7f454c46:
  case 0x160:
    little_endian = 0;
    break;
  case 0x464c457f:
  case 0x6001:
    little_endian = 1;
    break;
    
  default:
    little_endian = 0;
  }
}



  
unsigned int swap32(unsigned int x)
{
  if (little_endian)
  {
    return (((x & 0x000000FF) << 24) | 
	    ((x & 0x0000FF00) << 8)  |
	    ((x & 0x00FF0000) >> 8)  |
	    ((x & 0xFF000000) >> 24));
  }
  else
  {
    return x;
  }
}

unsigned short swap16(unsigned short x)
{
  if (little_endian)
  {
    return (((x & 0x00ff) << 8) |
	    ((x & 0xff00) >> 8));
  }
  else
  {
    return x;
  }
}
