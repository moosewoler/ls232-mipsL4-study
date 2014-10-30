/****************************************************************************
 *      $Id: append_ecoff.c,v 1.5 1998/12/29 06:19:07 gernot Exp $
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
#include "coff.h"
#include "elf.h"
#include "../../include/kernel/dit.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

void append_ecoff(char *imagename, char *appendname, int appendfile)
{
  FILHDR *chdr;
  AOUTHDR *ahdr;
  SCNHDR *shdr;
  Elf32_Ehdr *ehdr;
  Elf32_Phdr *phdr;
  Elf32_Shdr s32;
  Dit_Dhdr *dhdr;
  Dit_Phdr *dphdr;
  char *dbuff;
  
  int off, imagefile, padding, bsize, bp ;
  int r, i;
  unsigned int vaddr_base, file_offset_base, cur_file_offset;
  
  imagefile = open(imagename,O_RDWR);
  if (imagefile <0)
  {
    perror("while opening kernel image file");
    exit(1);
  }
  
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
  r = lseek(imagefile, off , SEEK_SET);
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



  /* print out header */
  chdr = malloc(sizeof(FILHDR));
  assert(chdr != NULL);

   r = lseek(appendfile, 0, SEEK_SET);
  assert(r == 0);

   r = read(appendfile,chdr,sizeof(FILHDR));
   assert (r == sizeof(FILHDR));

#if 0   
   printf("f_magic: 0x%x\n",  swap16(chdr->f_magic));
   printf("f_nscns: %d\n",  swap16(chdr->f_nscns));
   printf("f_timdat: 0x%x\n",  swap32(chdr->f_timdat));
   printf("f_symptr: 0x%x\n",  swap32(chdr->f_symptr));
   printf("f_nsyms: 0x%x\n", swap32(chdr->f_nsyms));
   printf("f_opthdr: 0x%x\n", swap16(chdr->f_opthdr));
   printf("f_flags: 0x%x\n", swap16(chdr->f_flags));
#endif
   assert(swap16(chdr->f_nscns) > 0);
   
   ahdr = malloc(sizeof(AOUTHDR));
   assert(ahdr != NULL);
  
   r = read(appendfile,ahdr,off = sizeof(AOUTHDR));
   assert (r == off);
   
#if 0   
   printf("magic: 0%o\n",swap16(ahdr->magic));
   printf("vstamp: %d\n",swap16(ahdr->vstamp));
   printf("tsize: 0x%x\n",swap32(ahdr->tsize));
   printf("dsize: 0x%x\n",swap32(ahdr->dsize));
   printf("bsize: 0x%x\n",swap32(ahdr->bsize));
   printf("entry: 0x%x\n",swap32(ahdr->entry));
   printf("text_start: 0x%x\n",swap32(ahdr->text_start));
   printf("data_start: 0x%x\n",swap32(ahdr->data_start));
   printf("bss_start: 0x%x\n",swap32(ahdr->bss_start));
   printf("gp_value: 0x%x\n",swap32(ahdr->gp_value));
#endif
   
   switch(swap16(ahdr->magic))
   {
   case OMAGIC:
   case NMAGIC:
   case ZMAGIC:
     break;
   default:
     fprintf(stderr,
	     "Error: unrecognised magic number in coff file (0%o)\n",
	     swap16(ahdr->magic));
     exit(1);
   }
   
   off = swap16(chdr->f_opthdr) + sizeof(FILHDR);
   shdr =  malloc(sizeof(SCNHDR));
   assert(shdr != NULL);
   
   dphdr = (Dit_Phdr *) &dbuff[swap32(dhdr->d_phnum)
			      * swap32(dhdr->d_phsize) +
			      swap32(dhdr->d_phoff)];
   dphdr->p_base = dphdr->p_size = 0;

   vaddr_base = swap32(dhdr->d_vaddrend);
   file_offset_base = swap32(dhdr->d_fileend);
   cur_file_offset = 0;
  
   
   for (i = 0; i < swap16(chdr->f_nscns); i++)
   {
     int pi, po;
     char buff[BUFF_SIZE];
  
     off = swap16(chdr->f_opthdr) + sizeof(FILHDR) + i * sizeof(SCNHDR);
     r = lseek(appendfile, off, SEEK_SET);
     assert(r == off);
     r = read(appendfile,shdr,sizeof(SCNHDR));
     assert (r == sizeof(SCNHDR));
#if 0     
     printf("**** section %d ****\n", i);
     printf("s_name: %s\n", shdr->s_name);
     printf("s_paddr: 0x%x\n", swap32(shdr->s_paddr));
     printf("s_vaddr: 0x%x\n", swap32(shdr->s_vaddr));
     printf("s_size: 0x%x\n", swap32(shdr->s_size));
     printf("s_scnptr: 0x%x\n", swap32(shdr->s_scnptr));
     printf("s_relptr: 0x%x\n", swap32(shdr->s_relptr));
     printf("s_lnnoptr: 0x%x\n", swap32(shdr->s_lnnoptr));
     printf("s_nreloc: 0x%x\n", swap16(shdr->s_nreloc));
     printf("s_nlnno: 0x%x\n", swap16(shdr->s_nlnno));
     printf("s_flags: 0x%x\n", swap32(shdr->s_flags));
#endif     

     switch(swap32(shdr->s_flags))
     {
     case STYP_TEXT:
     case STYP_DATA:
     case STYP_RDATA:
     case STYP_SDATA:
     case STYP_LIT8:
     case STYP_LIT4:
       /* something to load */
       if (dphdr->p_base == 0)
       {
	 dphdr->p_base = shdr->s_vaddr;
	 padding = 0;
	 fprintf(stderr,
		 "Appending %s (32-bit ecoff) to kernel image at 0x%lx\n",
		 appendname, swap32(shdr->s_vaddr));
	 if (dhdr->d_vaddrend != shdr->s_vaddr)
	 {
	   if (swap32(dhdr->d_vaddrend) < swap32(shdr->s_vaddr))
	   {
	     fprintf(stderr,
		     "Info: empty space at 0x%lx, binary linked at 0x%lx\n"
		     ,swap32(dhdr->d_vaddrend), swap32(shdr->s_vaddr));
	     
	     /* zero fill the space in between */
	     r = lseek(imagefile, file_offset_base, SEEK_SET);
	     assert(r == file_offset_base );

	     memset(buff,0, BUFF_SIZE);
	     padding = bsize = swap32(shdr->s_vaddr) -
	       swap32(dhdr->d_vaddrend);
	     bp = 0;
	    
	     do {
	       r = write(imagefile, buff, (bsize - bp) < BUFF_SIZE ?
			 (bsize - bp) : BUFF_SIZE);
	       assert(r >= 0);
	       bp += r;
	     } while (bp != bsize);
	     
	     vaddr_base =  swap32(shdr->s_vaddr);
	     file_offset_base += swap32(shdr->s_vaddr)
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
	 if ((swap32(shdr->s_vaddr) - vaddr_base) > cur_file_offset)
	 {
	   cur_file_offset = swap32(shdr->s_vaddr) - vaddr_base;
	 }
       }

       /* move to right offset in image file */
       r = lseek(imagefile, file_offset_base + cur_file_offset, SEEK_SET);
       assert(r == file_offset_base + cur_file_offset);
      
       r = lseek(appendfile, off = swap32(shdr->s_scnptr), SEEK_SET);
       assert(r == off);
      
       /* copy all the loadable segments into downloadable image file */
       
       pi = po = 0;
       bsize = swap32(shdr->s_size);
       do {
	 r = read(appendfile, buff,
		  (swap32(shdr->s_size) - po) < BUFF_SIZE ?
		  (swap32(shdr->s_size) - po) : BUFF_SIZE);
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
       cur_file_offset += swap32(shdr->s_size);
       break;

     case STYP_BSS:
     case STYP_SBSS:
       if (zero_bss)
	 {
	   /* something to alloc and pad with zero */
	   if ((swap32(shdr->s_vaddr) - vaddr_base) > cur_file_offset)
	     {
	       cur_file_offset = swap32(shdr->s_vaddr) - vaddr_base;
	     }
	   
	   /* move to right offset in image file */
	   r = lseek(imagefile, file_offset_base + cur_file_offset, SEEK_SET);
	   assert(r == file_offset_base + cur_file_offset);
	   
	   /* copy zeros into file */
	   for (pi = 0; pi < BUFF_SIZE; pi++)
	     {
	       buff[pi] = 0;
	     }
	   
	   bsize = swap32(shdr->s_vaddr);
	   po = 0;
	   
	   while (po != bsize)
	     {
	       r = write(imagefile, buff,
			 (bsize - po) < BUFF_SIZE ?
			 (bsize - po) : BUFF_SIZE);
	       assert(r >= 0);
	       po += r;
	     }
	   
	   
	   cur_file_offset += swap32(shdr->s_size);
	 }
       break;
     default:
       assert("unknown section in coff file" == 0);
     }
     
   }

   cur_file_offset = (cur_file_offset + 4095) & (~(unsigned int) 4095);
   
   dphdr->p_size = swap32(cur_file_offset);
   dphdr->p_entry = ahdr->entry;
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

   
   exit(0);
}
  
