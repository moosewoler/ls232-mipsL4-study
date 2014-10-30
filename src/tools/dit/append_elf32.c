/****************************************************************************
 *      $Id: append_elf32.c,v 1.6 1999/03/12 00:44:52 gernot Exp $
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
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

void check_elf32(Elf32_Ehdr *ehdr)
{


  /* now check if we have the right file type */
  if (ehdr->e_ident[EI_MAG0] != 0x7f ||
      ehdr->e_ident[EI_MAG1] != 'E' ||
      ehdr->e_ident[EI_MAG2] != 'L' ||
      ehdr->e_ident[EI_MAG3] != 'F')
  {
    fprintf(stderr,"invalid file type, expecting elf format\n");
    exit(1);
  }

  /* check if 32 bit elf*/
  if(ehdr->e_ident[EI_CLASS] != ELFCLASS32)
  {
	fprintf(stderr,"file not correct class (elf32)\n");
	exit(1);
  }

  /* check big endian */
  if(ehdr->e_ident[EI_DATA]!= ELFDATA2MSB)
  {
    fprintf(stderr,"expecting ELFDATA2MSB, got something else?\n");
    exit(1);
  }

  /* check for a mips */
  if (swap16(ehdr->e_machine) != EM_MIPS)
  {
    fprintf(stderr,"not a mips binary???\n");
    exit(1);
  }

  /* check file is an executable */
  if (swap16(ehdr->e_type) != ET_EXEC)
  {
    fprintf(stderr,"file not executable\n");
    exit(1);
  }

  if ((swap32(ehdr->e_flags) & ~(EF_MIPS_ABI2|EF_MIPS_NOREORDER)) !=
      EF_MIPS_ARCH_3)
  {
    fprintf(stderr,"expecting mips3 e_flag, got %x instead\n",
	    swap32(ehdr->e_flags));
    exit(1);
  }
}

void append_32_file(char *imagename, char *appendname, int appendfile)
{
  int r, imagefile, bp, bsize, i, padding;
  unsigned int vaddr_base, file_offset_base, cur_file_offset, off;
  Elf32_Ehdr *ehdr;
  Elf32_Phdr *phdr;
  Elf32_Ehdr *e32;
  Elf32_Phdr *p32;
  char *p64buff;


  Dit_Dhdr *dhdr;
  Dit_Phdr *dphdr;
  char *dbuff;
  Elf32_Shdr s32;
  /* open file */

  imagefile = open(imagename,O_RDWR);
  if (imagefile <0)
  {
    perror("while opening kernel image file");
    exit(1);
  }


  /* check if elf 32 */

  ehdr = malloc(sizeof(Elf32_Ehdr));
  assert(ehdr != NULL);

  r = read(imagefile,ehdr,sizeof(Elf32_Ehdr));

  assert(r == sizeof(Elf32_Ehdr)); /* assume we get enough
				      to read file hdr */
  set_endian(*(unsigned int *)ehdr);
  check_elf32(ehdr);


  /* move to the last program header table and read in */

  assert(swap16(ehdr->e_phnum) > 1);
  off = swap32(ehdr->e_phoff) + (swap16(ehdr->e_phnum) -1) *
    swap16(ehdr->e_phentsize);
  r = lseek(imagefile, off, SEEK_SET);
  assert(r == off);

  phdr = malloc(sizeof(Elf32_Phdr));
  assert(phdr != NULL);

  r = read(imagefile,phdr,sizeof(Elf32_Phdr));

  assert(r == sizeof(Elf32_Phdr)); /* assume we get enough
				      to read file hdr */

  /* now move to DIT header and read in */

  r = lseek(imagefile, off = swap32(phdr->p_offset), SEEK_SET);
  assert(r == off);


  dhdr = (Dit_Dhdr *) malloc(DHDR_SEG_SIZE);
  dbuff = (char *)dhdr;
  assert(dhdr != NULL);

  r = read(imagefile,dhdr,DHDR_SEG_SIZE);

  assert(r == DHDR_SEG_SIZE); /* assume we get enough
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



  /* check if elf 32 */

  e32 = malloc(sizeof(Elf32_Ehdr));
  assert(ehdr != NULL);

  r = lseek(appendfile, 0, SEEK_SET);
  assert(r == 0);
  r = read(appendfile,e32,sizeof(Elf32_Ehdr));

  assert(r == sizeof(Elf32_Ehdr)); /* assume we get enough
				      to read file hdr */
  check_elf32(e32);


  /* read in the program headers */
  /* move to start */

  r = lseek(appendfile, off = swap32(e32->e_phoff), SEEK_SET);
  assert(r == off);

    /* alloc space */
  p64buff = calloc(swap16(e32->e_phnum),swap16(e32->e_phentsize));
  assert(p64buff != NULL);

  /* read them in */

  bp = 0;
  bsize = swap16(e32->e_phnum) * swap16(e32->e_phentsize);
  do {
    r = read(appendfile, &p64buff[bp], bsize - bp);
    assert(r >= 0);
    bp += r;
  } while (bp != bsize);


  dphdr = (Dit_Phdr *) &dbuff[swap32(dhdr->d_phnum)
			     * swap32(dhdr->d_phsize) +
			     swap32(dhdr->d_phoff)];
  dphdr->p_base = dphdr->p_size = 0;

  vaddr_base = swap32(dhdr->d_vaddrend);
  file_offset_base = swap32(dhdr->d_fileend);
  cur_file_offset = 0;

  for (i = 0; i < swap16(e32->e_phnum); i++)
  {
    p32 = (Elf32_Phdr *) &p64buff[i * swap16(e32->e_phentsize)];
    if (p32->p_type == swap32(PT_LOAD))
    {
      int pi, po;
      char buff[BUFF_SIZE];

      /* we have a segment to load so copy segment into new file */
      if (dphdr->p_base == 0)
      {
	dphdr->p_base = p32->p_vaddr;
	padding = 0;
	fprintf(stderr, "Appending %s (32-bit elf) to kernel image at 0x%lx\n",
		appendname, swap32(p32->p_vaddr));
	if (dhdr->d_vaddrend != p32->p_vaddr)
	{
	  if (swap32(dhdr->d_vaddrend) < swap32(p32->p_vaddr))
	  {
	    fprintf(stderr,
		    "Info: empty space at 0x%lx, binary linked at 0x%lx\n",
		    swap32(dhdr->d_vaddrend), swap32(p32->p_vaddr));

	    /* zero fill the space in between */
	    r = lseek(imagefile, file_offset_base, SEEK_SET);
	    assert(r == file_offset_base );

	    memset(buff,0, BUFF_SIZE);
	    padding = bsize = swap32(p32->p_vaddr) -
	      swap32(dhdr->d_vaddrend);
	    bp = 0;

	    do {
	      r = write(imagefile, buff, (bsize - bp) < BUFF_SIZE ?
			(bsize - bp) : BUFF_SIZE);
	      assert(r >= 0);
	      bp += r;
	    } while (bp != bsize);

	    vaddr_base =  swap32(p32->p_vaddr);
	    file_offset_base += swap32(p32->p_vaddr)
	      - swap32(dhdr->d_vaddrend);
	  }
	  else
	  {
	    fprintf(stderr,
		    "Error: Attempt to append binary before end of kernel image\n");
	    exit(1);
	  }
	}
      }
      else
      {
	if ((swap32(p32->p_vaddr) - vaddr_base) > cur_file_offset)
	{
	  cur_file_offset = swap32(p32->p_vaddr) - vaddr_base;
	}
      }
      assert(p32->p_paddr == p32->p_vaddr);
      assert(p32->p_align == swap32(DHDR_ALIGN));

      /* move to right offset in image file */
      r = lseek(imagefile, file_offset_base + cur_file_offset, SEEK_SET);
      assert(r == file_offset_base + cur_file_offset);

      r = lseek(appendfile, off = swap32(p32->p_offset), SEEK_SET);
      assert(r == off);

      /* copy all the loadable segments into downloadable image file */

      pi = po = 0;
      bsize = swap32(p32->p_filesz);
      do {
	r = read(appendfile, buff,
		 (swap32(p32->p_filesz) - po) < BUFF_SIZE ?
		 (swap32(p32->p_filesz) - po) : BUFF_SIZE);
	assert(r >= 0);

	pi = r;
	bp = 0;

	while (bp != pi)
	{
	  r = write(imagefile, &buff[bp], pi - bp);
	  assert(r >= 0);
	  bp += r;
	  po += r;
	}
      } while (po != bsize);

      /* now zero fill between end of filesz and memsz */
      for (pi = 0; pi < BUFF_SIZE; pi++)
      {
	buff[pi] = 0;
      }

      bsize = ((swap32(p32->p_memsz) | (4096 - 1)) + 1)
	- swap32(p32->p_filesz);
      po = 0;

      r = lseek(imagefile,
		off = file_offset_base + cur_file_offset +
		swap32(p32->p_filesz),
		SEEK_SET);
      assert(r == off);

      while (po != bsize)
      {
	r = write(imagefile, buff,
		  (bsize - po) < BUFF_SIZE ?
		  (bsize - po) : BUFF_SIZE);
	assert(r >= 0);
	po += r;
      }

      cur_file_offset +=  ((swap32(p32->p_memsz) | (4096 - 1)) + 1);
    }

  }

  dphdr->p_size = swap32(cur_file_offset);
  dphdr->p_entry = e32->e_entry;
  strncpy(dphdr->p_name, appendname, DIT_NPNAME);
  dphdr->p_name[DIT_NPNAME - 1] = 0;
  dphdr->p_flags = swap32(flags);
  dhdr->d_phnum = swap32(swap32(dhdr->d_phnum) + 1);
  dhdr->d_fileend = swap32(file_offset_base + cur_file_offset);
  dhdr->d_vaddrend = swap32(vaddr_base + cur_file_offset);


  /* now write DIT header back */
  r = lseek(imagefile, off = swap32(phdr->p_offset), SEEK_SET);
  assert(r == off);

  bp = 0;
  bsize =  DHDR_SEG_SIZE;
  do {
    r = write(imagefile, &dbuff[bp], bsize - bp);
    assert(r >= 0);
    bp += r;
  } while (bp != bsize);
  /* now patch the original image program header */
  r = lseek(imagefile,
	    off = swap32(ehdr->e_phoff) + (swap16(ehdr->e_phnum) -1)
	    * swap16(ehdr->e_phentsize)
	    , SEEK_SET);
  assert(r == off);

  phdr->p_filesz = swap32(swap32(phdr->p_filesz) + padding + cur_file_offset);
  phdr->p_memsz = swap32(swap32(phdr->p_memsz) + padding + cur_file_offset);

  r = write(imagefile,phdr,sizeof(Elf32_Phdr));


  /* tack a section header on the end */

  assert(r == sizeof(Elf32_Phdr)); /* assume we get enough
				      to read file hdr */
  s32.sh_name = 0;
  s32.sh_type = swap32(SHT_NULL);
  s32.sh_flags = 0;
  s32.sh_addr = 0;
  s32.sh_offset = 0;
  s32.sh_size = 0;
  s32.sh_link = swap32(SHN_UNDEF);
  s32.sh_info = 0;
  s32.sh_addralign = 0;
  s32.sh_entsize = 0;

  r = lseek(imagefile, off = swap32(dhdr->d_fileend), SEEK_SET);
  assert(r == off);

  bp = 0;
  bsize = swap16(ehdr->e_shentsize);
  do {
    r = write(imagefile, &((char *)(&s32))[bp], bsize - bp);
    assert(r >= 0);
    bp += r;
  } while (bp != bsize);

  /* fix initial header */

  ehdr->e_shoff = dhdr->d_fileend;

  r = lseek(imagefile, 0, SEEK_SET);
  assert(r == 0);

  bp = 0;
  bsize = swap16(ehdr->e_ehsize);
  do {
    r = write(imagefile, &((char *)(ehdr))[bp], bsize - bp);
    assert(r >= 0);
    bp += r;
  } while (bp != bsize);


  close(imagefile);

}
