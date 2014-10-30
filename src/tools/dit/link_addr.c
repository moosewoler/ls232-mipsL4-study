/****************************************************************************
 *      $Id: link_addr.c,v 1.3 1998/01/27 01:10:09 kevine Exp $
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
#include "protos.h"
#include "../../include/kernel/dit.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

void show_link_addr(int c, char *v[])
{
  char *infilename;
  int r, infile;
  Elf32_Ehdr *ehdr;
  Elf32_Phdr *phdr;
  
  Dit_Dhdr *dhdr;

  /* open file */
  if (c != 1)
  {
    usage();
  }
  
  infilename = v[0];
  
  infile = open(infilename,O_RDONLY);
  if (infile <0)
  {
    perror("while opening infile");
    exit(1);
  }

  
  /* check if elf 32 */
  
  ehdr = malloc(sizeof(Elf32_Ehdr));
  assert(ehdr != NULL);
   
  r = read(infile,ehdr,sizeof(Elf32_Ehdr));
    
  assert(r == sizeof(Elf32_Ehdr)); /* assume we get enough
				      to read file hdr */
  set_endian(*(unsigned int *)ehdr);
  check_elf32(ehdr);


  /* move to the last program header table and read in */

  assert(swap16(ehdr->e_phnum) > 1);
  r = lseek(infile, swap32(ehdr->e_phoff) + (swap16(ehdr->e_phnum) -1)
	    * swap16(ehdr->e_phentsize)
	    , SEEK_SET);
  assert(r == swap32(ehdr->e_phoff) + (swap16(ehdr->e_phnum) -1) *
	 swap16(ehdr->e_phentsize));
  
  phdr = malloc(sizeof(Elf32_Phdr));
  assert(phdr != NULL);

  r = read(infile,phdr,sizeof(Elf32_Phdr));
    
  assert(r == sizeof(Elf32_Phdr)); /* assume we get enough
				      to read file hdr */

  /* now move to DIT header and read in */
  
  r = lseek(infile, swap32(phdr->p_offset), SEEK_SET);
  assert(r == swap32(phdr->p_offset));

  
  dhdr = malloc(sizeof(Dit_Dhdr));
  assert(dhdr != NULL);

  r = read(infile,dhdr,sizeof(Dit_Dhdr));
    
  assert(r == sizeof(Dit_Dhdr)); /* assume we get enough
				      to read file hdr */

  /* now check if correct format */

  if (dhdr->d_ident[0] != 'd' ||
      dhdr->d_ident[1] != 'h' ||
      dhdr->d_ident[2] != 'd' ||
      dhdr->d_ident[3] != 'r')
  {
    fprintf(stderr,"file doesn't contain dhdr\n");
    exit(1);
  }

  
  /* okay, print out link address !! */
  

  printf("0x%.8x\n",swap32(dhdr->d_vaddrend));

}
