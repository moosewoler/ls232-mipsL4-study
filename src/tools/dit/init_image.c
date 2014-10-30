/****************************************************************************
 *      $Id: init_image.c,v 1.3 1998/01/27 01:10:08 kevine Exp $
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



void build_kernel(int c, char *v[])
{
  char *infilename, *outfilename;
  int i, r, bp, bsize, loadcount;
  unsigned int fileend;
  unsigned int high_address;
  int infile, outfile, off;
  Elf64_Ehdr *ehdr;
  
  Elf64_Phdr *phdr;
  char *pbuff;
  
  Elf64_Shdr *shdr;
  char *sbuff;

  Dit_Dhdr *dhdr;
  char *dbuff;
  
  
  Elf32_Ehdr e32;
  Elf32_Phdr p32[6]; /* only six program segments supported by PMON */
  Elf32_Shdr s32 ; /* put in one null section to avoid breaking PMON */

  if (c != 2)
  {
    usage();
  }
  infilename = v[0];
  outfilename = v[1];
  
  infile = open(infilename,O_RDONLY);
  if (infile <0)
  {
    perror("while opening infile");
    exit(1);
  }

  ehdr = malloc(sizeof(Elf64_Ehdr));
  assert(ehdr != NULL);
    
  r = read(infile,ehdr,sizeof(Elf64_Ehdr));
    
  assert(r == sizeof(Elf64_Ehdr)); /* assume we get enough
				      to read file hdr */
  set_endian(*(unsigned int *)ehdr);
  check_elf64(ehdr);
  


  /* okay lets read in program headers */
  
  /* move to start */
  assert(ehdr->e_phoff.hi32 == 0);
  r = lseek(infile, off = swap32(ehdr->e_phoff.lo32), SEEK_SET);
  assert(r == off);
  
    /* alloc space */
  pbuff = calloc(swap16(ehdr->e_phnum) + 1,swap16(ehdr->e_phentsize));
  assert(pbuff != NULL);

  /* read them in */
  
  bp = 0;
  bsize = swap16(ehdr->e_phnum) * swap16(ehdr->e_phentsize);
  do {
    r = read(infile, &pbuff[bp], bsize - bp);
    assert(r >= 0);
    bp += r;
  } while (bp != bsize);
	
  /* okay lets read in section headers */

  /* move to start */
  assert(ehdr->e_shoff.hi32 == 0);
  r = lseek(infile, off = swap32(ehdr->e_shoff.lo32), SEEK_SET);
  assert(r == off);
  
  /* alloc space */
  sbuff = calloc(swap16(ehdr->e_shnum),swap16(ehdr->e_shentsize));
  assert(pbuff != NULL);
  
  /* read them in */
  
  bp = 0;
  bsize = swap16(ehdr->e_shnum) * swap16(ehdr->e_shentsize);
  do {
    r = read(infile, &sbuff[bp], bsize - bp);
    assert(r >= 0);
    bp += r;
  } while (bp != bsize);
  
  /* okay we have all the info, lets build a elf32 executable from the
     elf64 one */
  
  /* fill in what we can now in elf32 header*/
  
  e32.e_ident[EI_MAG0] = 0x7f;
  e32.e_ident[EI_MAG1] = 'E';
  e32.e_ident[EI_MAG2] = 'L';
  e32.e_ident[EI_MAG3] = 'F';
  e32.e_ident[EI_CLASS] = ELFCLASS32;
  e32.e_ident[EI_DATA] = ELFDATA2MSB;
  e32.e_ident[EI_VERSION] = EV_CURRENT;
  
  e32.e_type = swap16(ET_EXEC);
  e32.e_machine = swap16(EM_MIPS);
  e32.e_version = swap32(EV_CURRENT);
  e32.e_entry = ehdr->e_entry.lo32; /* assume linked somewhere that will
				       work */
  e32.e_flags = swap32(EF_MIPS_ARCH_3);
  e32.e_ehsize = swap16(sizeof(Elf32_Ehdr));
  e32.e_phentsize = swap16(sizeof(Elf32_Phdr));
  e32.e_shentsize = swap16(sizeof(Elf32_Shdr));
  e32.e_shstrndx = swap16(SHN_UNDEF);
  
  /* fill out section 0 */
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


  /* open the outfile now */
  outfile = open(outfilename,O_TRUNC | O_WRONLY | O_CREAT, 0644);
  if (outfile < 0)
  {
    perror("while opening outfile");
    exit(1);
  }
  
  /* Hmmm, now go through each program header in turn */
  loadcount = 0;
  fileend = 0;
  high_address = 0;
  
  for (i = 0; i < swap16(ehdr->e_phnum); i++)
  {
    phdr = (Elf64_Phdr *) &pbuff[i * swap16(ehdr->e_phentsize)];
    if (phdr->p_type == swap32(PT_LOAD))
    {
      int pi, po;
      char buff[BUFF_SIZE];
      
      /* we have a segment to load so build a elf32 phdr
	 and copy segment into new file */
      p32[loadcount].p_type = swap32(PT_LOAD);
      p32[loadcount].p_flags = phdr->p_flags;
      
      assert(phdr->p_offset.hi32 == 0);
      p32[loadcount].p_offset = phdr->p_offset.lo32;
      
      assert(phdr->p_vaddr.hi32 == 0xffffffff ||
	     phdr->p_vaddr.hi32 == 0x0);
      p32[loadcount].p_vaddr = phdr->p_vaddr.lo32;
      
      assert(phdr->p_paddr.hi32 == 0xffffffff ||
	     phdr->p_vaddr.hi32 == 0x0);
      p32[loadcount].p_paddr = phdr->p_paddr.lo32;

      assert(phdr->p_paddr.lo32 == phdr->p_vaddr.lo32);
      
      assert(phdr->p_filesz.hi32 == 0);
      p32[loadcount].p_filesz = phdr->p_filesz.lo32;
      
      assert(phdr->p_memsz.hi32 == 0);
      p32[loadcount].p_memsz = phdr->p_memsz.lo32;


      if (high_address < (off = swap32(phdr->p_vaddr.lo32)
			  +  swap32(phdr->p_memsz.lo32)))
      {
	high_address = off;
      }
      
      assert(phdr->p_align.hi32 == 0);
      p32[loadcount].p_align = phdr->p_align.lo32;
      
      r = lseek(outfile, off = swap32(p32[loadcount].p_offset), SEEK_SET);
      assert(r == off);
      
      r = lseek(infile, off = swap32(p32[loadcount].p_offset), SEEK_SET);
      assert(r == off);
      
      if ((off = swap32(p32[loadcount].p_offset)
	   + swap32(p32[loadcount].p_filesz)) > fileend)
      {
	fileend = off;
      }
      
      /* copy all the loadable segments into downloadable image file */

      pi = po = 0;
      bsize = swap32(p32[loadcount].p_filesz);
      do {
	r = read(infile, buff,
		 (swap32(p32[loadcount].p_filesz) - po) < BUFF_SIZE ?
		 (swap32(p32[loadcount].p_filesz) - po) : BUFF_SIZE);
	assert(r >= 0);
	
	pi = r;
	bp = 0;
	
	while (bp != pi)
	{
	  r = write(outfile, &buff[bp], pi - bp);
	  assert(r >= 0);
	  bp += r;
	  po += r;
	}
      } while (po != bsize);
      
      loadcount++;
    }
    
  }
  assert(loadcount > 0);

  /* build the loadable section that is the DIT header for other
     stuff */

  p32[loadcount].p_type = swap32(PT_LOAD);
  p32[loadcount].p_flags = swap32(PF_R);
  p32[loadcount].p_offset = swap32(fileend);
  p32[loadcount].p_vaddr = p32[loadcount].p_paddr = swap32(
    (high_address | (DHDR_ALIGN - 1)) + 1);
  p32[loadcount].p_filesz = p32[loadcount].p_memsz = swap32(DHDR_SEG_SIZE);
  p32[loadcount].p_align = swap32(DHDR_ALIGN);

  /* make the header*/
  dbuff = malloc(DHDR_SEG_SIZE);
  dhdr = (Dit_Dhdr *) dbuff;
  dhdr->d_ident[0] = 'd';
  dhdr->d_ident[1] = 'h';
  dhdr->d_ident[2] = 'd';
  dhdr->d_ident[3] = 'r';

  dhdr->d_phoff = swap32(sizeof(Dit_Dhdr));
  dhdr->d_phsize = swap32(sizeof(Dit_Phdr));
  dhdr->d_phnum = 0;
  fileend += DHDR_SEG_SIZE;
  dhdr->d_fileend = swap32(fileend);
  dhdr->d_vaddrend = swap32(( swap32(p32[loadcount].p_vaddr)
			      + DHDR_SEG_SIZE ) & 0x07ffffff);
  

  /* now lets fix up the rest of the headers and write them out */

  loadcount++;
  
  e32.e_phoff = swap32(swap16(e32.e_ehsize));
  e32.e_phnum = swap16(loadcount);
  e32.e_shoff = swap32(fileend);
  e32.e_shnum =  swap16(1);

  
  /* write out the elf header */
  
  r = lseek(outfile, 0, SEEK_SET);
  assert(r == 0);
  
  bp = 0;
  bsize = swap16(e32.e_ehsize);
  do {
    r = write(outfile, &((char *)(&e32))[bp], bsize - bp);
    assert(r >= 0);
    bp += r;
  } while (bp != bsize);



  /* write the null segment header last in the file */
  
  r = lseek(outfile, fileend, SEEK_SET);
  assert(r == fileend);
  
  bp = 0;
  bsize = swap16(e32.e_shentsize);
  do {
    r = write(outfile, &((char *)(&s32))[bp], bsize - bp);
    assert(r >= 0);
    bp += r;
  } while (bp != bsize);
  


  /* write out the program headers after starting elf header */
  
  r = lseek(outfile, swap16(e32.e_ehsize), SEEK_SET);
  assert(r == swap16(e32.e_ehsize));


  for (i = 0; i < loadcount; i++)
  {
    bp = 0;
    bsize = swap16(e32.e_phentsize);
    do {
      r = write(outfile, &((char *)(&p32[i]))[bp], bsize - bp);
      assert(r >= 0);
      bp += r;
    } while (bp != bsize);
  }

  /* all program segments except the DIT header have been written out above,
     so now write the DIT header */

  r = lseek(outfile, off = swap32(p32[loadcount-1].p_offset), SEEK_SET);
  assert(r == off);

  
  bp = 0;
  bsize =  DHDR_SEG_SIZE;
  do {
    r = write(outfile, &dbuff[bp], bsize - bp);
    assert(r >= 0);
    bp += r;
  } while (bp != bsize);
  
  close(outfile);
  close(infile);
}

