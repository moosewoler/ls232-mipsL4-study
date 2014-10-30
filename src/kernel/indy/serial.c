/****************************************************************************
 * $Id: serial.c,v 1.2 1998/01/22 05:47:20 kevine Exp $
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
unsigned char rdzero(void);
unsigned char rdreg(unsigned char);
void wrreg(unsigned char, unsigned char);


void outch(int b)
{
  while (!(rdzero() & 4));
  wrreg(8,(unsigned char) b);

}

int inch(void)
{

  while(!(rdzero() & 1));
  return rdreg(8);
}

