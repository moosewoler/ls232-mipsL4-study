/****************************************************************************
 *      $Id: dit.c,v 1.13 1999/03/12 00:44:53 gernot Exp $
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

/* horrible hack to get sys/types.h to you struct
		     for 64 bit words */
#define m68000
#define DEBUG
#include <stdio.h>
#include "elf.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include "../../include/kernel/dit.h"
#include "protos.h"



/* void build_kernel(char *infilename, char *outfilename);
 * void show_link_addr(char *filename);
 * void append_file(char *appendfilename ,char* outfilename);
 * void dump_dit_header(char *imagefilename); 
 */

void usage(void)
{
  fprintf(stderr, "DIT $Revision: 1.13 $\n");
  fprintf(stderr, "Usage: dit -i elf64_file kernel_image\n");
  fprintf(stderr, "     : dit -l kernel_image\n");
  fprintf(stderr, "     : dit [-h addr] [-f] [-n] [-z] -a file kernel_image\n");
  fprintf(stderr, "     : dit -m kernel_image\n");
  exit(1);
}
int flags;
int force_data;
int zero_bss;

unsigned int faddr;

main(int argc, char *argv[])
{
  char *argv0;
  int c, r;
  char **v;
  void (*func)(int c, char *v[]);


  argv0 = argv[0];
  flags = DIT_RUN;
  force_data = 0;
  zero_bss = 1;
  faddr = 0;
  little_endian = 0; /* assume big for now */
  argv++;
  argc--;
  
  /* process args */

  if (argc <= 1)
  {
    usage();
  }

  if (argv[0][0] != '-')
  {
    usage();
  }

  while (argc != 0)
  {
    switch(argv[0][1])
    {
    case 'h':
      argv++;
      argc--;
      r = sscanf(argv[0],"%x", &faddr);
      if (r != 1)
      {
	faddr = 0;
      }
      argv++;
      argc--;
      break;
    case 'a':
      argv++;
      argc--;
      func = append_file;
      c = argc;
      v = argv;
      argv++; argv++;
      argc--;argc--;
      break;
    case 'n':
      flags = flags & (~ DIT_RUN);
      argv++;
      argc--;
      break;
    case 'f':
      force_data = 1;
      argv++;
      argc--;
      break;
    case 'z':
      zero_bss = 0;
      argv++;
      argc--;
      break;
    case 'i':
      argv++;
      argc--;
      func = build_kernel;
      c = argc;
      v = argv;
      argv++; argv++;
      argc--;argc--;
      break;
    case 'm':
      argv++;
      argc--;
      func = dump_dit_header;
      c = argc;
      v = argv;
      argv++; argc--;
      break;
    case 'l':
      argv++;
      argc--;
      func = show_link_addr;
      c = argc;
      v = argv;
      argv++; argc--;
      break;
    default:
      usage();
    }
  }
  (*func)(c,v);
  exit(0);
}


void append_file(int c, char *v[])
{
  char *appendname, *imagename, buff[16];
  int r, imagefile, appendfile;
  
  if (c != 2)
  {
    usage();
  }
  appendname = v[0];
  imagename = v[1];

  appendfile = open(appendname,O_RDONLY);
  if (appendfile <0)
  {
    perror("while opening appendfile");
    exit(1);
  }
  if (force_data)
  {
    /* unrecognised format, assume data */
    flags = flags & (~ DIT_RUN);
    append_data(imagename, appendname, appendfile);
  }
  else
  {
    r = read(appendfile,buff,16); /* minimum file size is 16 bytes */
    assert(r == 16);
    if (
	(*(unsigned int *)buff == 0x7f454c46) || /* ELF */
	(*(unsigned int *)buff == 0x464c457f)
	)
    {
      if (((Elf64_Ehdr *)buff) -> e_ident[EI_CLASS] == ELFCLASS64)
      {
	append_64_file(imagename, appendname, appendfile);
      }
      else if (((Elf32_Ehdr *)buff) -> e_ident[EI_CLASS] == ELFCLASS32)
      {
	append_32_file(imagename, appendname, appendfile);
      }
      else
      {
	fprintf(stderr,"Error: file not 64-bit or 32-bit ELF\n");
	exit(1);
      }
    }
    else if ((*(unsigned short *) buff == 0x0160) || /* COFF */
	     (*(unsigned short *) buff == 0x6001)
	     )
    {
      append_ecoff(imagename, appendname, appendfile);
    }
    else
    {
      /* unrecognised format, assume data */
      flags = flags & (~ DIT_RUN);
      append_data(imagename, appendname, appendfile);
    }
  }
  close(appendfile);
}
