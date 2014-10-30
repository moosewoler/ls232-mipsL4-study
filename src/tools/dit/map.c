/****************************************************************************
 *      $Id: map.c,v 1.4 1998/01/27 01:10:10 kevine Exp $
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

void dump_dit_header(int c, char *v[])
{
  char *imagefilename;
  int r, infile, off;
  Elf32_Ehdr *ehdr;
  Elf32_Phdr *phdr;
  
  Dit_Dhdr *dhdr;
  char *dbuff;
  Dit_Phdr *dphdr;
  
  /* open file */
  if (c != 1)
  {
    usage();
  }
  imagefilename = v[0];
  infile = open(imagefilename,O_RDONLY);
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
  off = swap32(ehdr->e_phoff) + (swap16(ehdr->e_phnum) -1) *
    swap16(ehdr->e_phentsize);
  r = lseek(infile, off, SEEK_SET);
  assert(r == off);
  
  phdr = malloc(sizeof(Elf32_Phdr));
  assert(phdr != NULL);

  r = read(infile,phdr,sizeof(Elf32_Phdr));
    
  assert(r == sizeof(Elf32_Phdr)); /* assume we get enough
				      to read file hdr */

  /* now move to DIT header and read in */
  
  r = lseek(infile, swap32(phdr->p_offset), SEEK_SET);
  assert(r == swap32(phdr->p_offset));

  
  dhdr = malloc(DHDR_SEG_SIZE);
  dbuff = (char *) dhdr;
  assert(dhdr != NULL);

  r = read(infile,dhdr,DHDR_SEG_SIZE);
    
  assert(r == DHDR_SEG_SIZE); /* assume we get enough
				      to read file hdr */

  /* now check if correct format */

  if (dhdr->d_ident[0] != 'd' ||
      dhdr->d_ident[1] != 'h' ||
      dhdr->d_ident[2] != 'd' ||
      dhdr->d_ident[3] != 'r')
  {
    fprintf(stderr,"file doesn't contain dhdr, not a dit format file\n");
    exit(1);
  }


/* okay, print out details !! */

  printf("          ***** DIT HEADER *****\n");
  printf("Offset              Size             Number\n");
  printf("FileEnd             VaddrEnd\n");
  printf("\n0x%.8x          0x%.8lx       %d\n",
	 swap32(dhdr->d_phoff),
	 swap32(dhdr->d_phsize),
	 swap32(dhdr->d_phnum));
  printf("0x%.8x          0x%.8x\n",
	 swap32(dhdr->d_fileend),
	 swap32(dhdr->d_vaddrend));
  if (swap32(dhdr->d_phnum) > 0)
  {
    int i;
    printf("\n");
    printf("    ***** DIT PROGRAM HEADERS *****\n");
    printf("Base         Size         Entry        Flags     Name\n");      
    
    for (i = 0; i < swap32(dhdr->d_phnum); i++)
    {
      dphdr = (Dit_Phdr *) &dbuff[i * swap32(dhdr->d_phsize)
				 + swap32(dhdr->d_phoff)];
      printf("\n0x%.8x   0x%.8x   0x%.8x   0x%.4x   %s\n",
	     swap32(dphdr->p_base),
	     swap32(dphdr->p_size),
	     swap32(dphdr->p_entry),
	     swap32(dphdr->p_flags),
	     dphdr->p_name);
    }
  }
  close(infile);
}
