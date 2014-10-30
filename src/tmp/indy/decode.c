/****************************************************************************
 *      $Id: decode.c,v 1.2 1998/01/22 05:50:20 kevine Exp $
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
void wrreg(unsigned char reg, unsigned char val);
unsigned char rdreg(unsigned char reg);
unsigned char rdzero(void);

void outch(unsigned char b)
{
  int i;
  while (!(rdzero() & 4))
    ;
  wrreg(8,b);
}

void delay(unsigned char b)
{
  int i;
  outch(b);
  for (i = 0; i < 100000000; i++);
}

void decode(void)
{
  int i;
  unsigned char b, r4, r5, r12, r13;

  b = rdzero();
  wrreg(1,0);
  wrreg(9,0);
  wrreg(15,0);
  wrreg(7,0x40);
  r4 = rdreg(4);
  r5 = rdreg(0);
  r12 = rdreg(12);
  r13 = rdreg(13);
  
  outch('4');
  outch('>');
  outch(' ');
  b = r4;
  for (i = 7; i >= 0; i--)
  {
    if (b >> i & 1)
    {
      outch('1');
    }
    else
    {
      outch('0');
    }
  }
  outch('\n');
  outch('\r');

  outch('5');
  outch('>');
  outch(' ');
  b = r5;
  for (i = 7; i >= 0; i--)
  {
    if (b >> i & 1)
    {
      outch('1');
    }
    else
    {
      outch('0');
    }
  }
  outch('\n');
  outch('\r');

  outch('1');
  outch('2');
  outch('>');
  outch(' ');
  b = r12;
  for (i = 7; i >= 0; i--)
  {
    if (b >> i & 1)
    {
      outch('1');
    }
    else
    {
      outch('0');
    }
  }
  outch('\n');
  outch('\r');

  outch('1');
  outch('3');
  outch('>');
  outch(' ');
  b = r13;
  for (i = 7; i >= 0; i--)
  {
    if (b >> i & 1)
    {
      outch('1');
    }
    else
    {
      outch('0');
    }
  }
  outch('\n');
  outch('\r');

  while (1)
  {
    while (!(rdzero() & 1));

    b = rdreg(8);
    outch(b);
  }

}
