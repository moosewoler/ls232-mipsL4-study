#ifndef _PROTOS_H
#define _PROTOS_H
/****************************************************************************
 *      $Id: protos.h,v 1.4 1999/03/12 00:44:53 gernot Exp $
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

#include "elf.h"

#define BUFF_SIZE 1024

extern int little_endian;
extern int flags;
extern int zero_bss;
extern unsigned int faddr;

extern unsigned int swap32(unsigned int b);
extern unsigned short swap16(unsigned short);
extern void set_endian(unsigned int);
extern void check_elf32(Elf32_Ehdr *ehdr);
extern void check_elf64(Elf64_Ehdr *ehdr);

extern void append_32_file(char *, char *, int);
extern void append_data(char *, char *, int);
extern void append_64_file(char *, char *, int);
extern void append_ecoff(char *, char *, int);

extern void dump_dit_header(int c, char *v[]);
extern void show_link_addr(int c, char *v[]);
extern void build_kernel(int c, char *v[]);
extern void append_file(int c, char *v[]);



#endif
