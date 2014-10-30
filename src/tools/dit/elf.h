#ifndef _ELF_H
#define _ELF_H
/****************************************************************************
 *      $Id: elf.h,v 1.2 1998/01/22 05:50:33 kevine Exp $
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

/* define some machine independent types to later define elf types */

typedef unsigned char _elf_uint8;
typedef char _elf_int8;
typedef unsigned short _elf_uint16;
typedef short _elf_int16;
typedef unsigned int _elf_uint32;
typedef int _elf_int32;
typedef struct {
  unsigned int hi32;
  unsigned int lo32;
}  _elf_uint64;
typedef struct {        
  int hi32;
  int lo32;
} _elf_int64;


/* define elf types */
/* 32-bit */
typedef _elf_uint32     Elf32_Addr;
typedef _elf_uint16     Elf32_Half;
typedef _elf_uint32     Elf32_Off;
typedef _elf_int32      Elf32_Sword;
typedef _elf_uint32     Elf32_Word;
typedef _elf_uint8      Elf32_Byte;     
typedef _elf_uint16     Elf32_Section;  

/* 64-bit */
typedef _elf_uint64    Elf64_Addr;
typedef _elf_uint16    Elf64_Half;
typedef _elf_uint64    Elf64_Off;
typedef _elf_int32     Elf64_Sword;
typedef _elf_int64     Elf64_Sxword;
typedef _elf_uint32    Elf64_Word;
typedef _elf_uint64    Elf64_Xword;
typedef _elf_uint8     Elf64_Byte;     /* unsigned tiny integer */
typedef _elf_uint16    Elf64_Section;  /* section index (unsigned) */


/* ELF Header (32-bit implementations) */
#define EI_NIDENT       16

typedef struct {
        unsigned char   e_ident[EI_NIDENT];     /* ident bytes */
        Elf32_Half      e_type;                 /* file type */
        Elf32_Half      e_machine;              /* target machine */
        Elf32_Word      e_version;              /* file version */
        Elf32_Addr      e_entry;                /* start address */
        Elf32_Off       e_phoff;                /* phdr file offset */
        Elf32_Off       e_shoff;                /* shdr file offset */
        Elf32_Word      e_flags;                /* file flags */
        Elf32_Half      e_ehsize;               /* sizeof ehdr */
        Elf32_Half      e_phentsize;            /* sizeof phdr */
        Elf32_Half      e_phnum;                /* number phdrs */
        Elf32_Half      e_shentsize;            /* sizeof shdr */
        Elf32_Half      e_shnum;                /* number shdrs */
        Elf32_Half      e_shstrndx;             /* shdr string index */
} Elf32_Ehdr;


typedef struct {
        unsigned char   e_ident[EI_NIDENT];     /* ident bytes */
        Elf64_Half      e_type;                 /* file type */
        Elf64_Half      e_machine;              /* target machine */
        Elf64_Word      e_version;              /* file version */
        Elf64_Addr      e_entry;                /* start address */
        Elf64_Off       e_phoff;                /* phdr file offset */
        Elf64_Off       e_shoff;                /* shdr file offset */
        Elf64_Word      e_flags;                /* file flags */
        Elf64_Half      e_ehsize;               /* sizeof ehdr */
        Elf64_Half      e_phentsize;            /* sizeof phdr */
        Elf64_Half      e_phnum;                /* number phdrs */
        Elf64_Half      e_shentsize;            /* sizeof shdr */
        Elf64_Half      e_shnum;                /* number shdrs */
        Elf64_Half      e_shstrndx;             /* shdr string index */
} Elf64_Ehdr;

/* Program header */

typedef struct {
        Elf32_Word      p_type;         /* entry type */
        Elf32_Off       p_offset;       /* file offset */
        Elf32_Addr      p_vaddr;        /* virtual address */
        Elf32_Addr      p_paddr;        /* physical address */
        Elf32_Word      p_filesz;       /* file size */
        Elf32_Word      p_memsz;        /* memory size */
        Elf32_Word      p_flags;        /* entry flags */
        Elf32_Word      p_align;        /* memory/file alignment */
} Elf32_Phdr;

typedef struct {
        Elf64_Word      p_type;         /* entry type */
        Elf64_Word      p_flags;        /* entry flags */
        Elf64_Off       p_offset;       /* file offset */
        Elf64_Addr      p_vaddr;        /* virtual address */
        Elf64_Addr      p_paddr;        /* physical address */
        Elf64_Xword     p_filesz;       /* file size */
        Elf64_Xword     p_memsz;        /* memory size */
        Elf64_Xword     p_align;        /* memory/file alignment */
} Elf64_Phdr;

/* Section header */

typedef struct {
        Elf32_Word      sh_name;        /* section name */
        Elf32_Word      sh_type;        /* SHT_... */
        Elf32_Word      sh_flags;       /* SHF_... */
        Elf32_Addr      sh_addr;        /* virtual address */
        Elf32_Off       sh_offset;      /* file offset */
        Elf32_Word      sh_size;        /* section size */
        Elf32_Word      sh_link;        /* misc info */
        Elf32_Word      sh_info;        /* misc info */
        Elf32_Word      sh_addralign;   /* memory alignment */
        Elf32_Word      sh_entsize;     /* entry size if table */
} Elf32_Shdr;

typedef struct {
        Elf64_Word      sh_name;        /* section name */
        Elf64_Word      sh_type;        /* SHT_... */
        Elf64_Xword     sh_flags;       /* SHF_... */
        Elf64_Addr      sh_addr;        /* virtual address */
        Elf64_Off       sh_offset;      /* file offset */
        Elf64_Xword     sh_size;        /* section size */
        Elf64_Word      sh_link;        /* misc info */
        Elf64_Word      sh_info;        /* misc info */
        Elf64_Xword     sh_addralign;   /* memory alignment */
        Elf64_Xword     sh_entsize;     /* entry size if table */
} Elf64_Shdr;


/* constants */

#define EI_MAG0         0               /* e_ident[] indexes */
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4
#define EI_DATA         5
#define EI_VERSION      6
#define EI_PAD          7

#define ELFMAG0         0x7f            /* EI_MAG */
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'
#define ELFMAG          "\177ELF"
#define SELFMAG         4
#define ELFCLASSNONE    0               /* EI_CLASS */
#define ELFCLASS32      1
#define ELFCLASS64      2
#define ELFCLASSNUM     3

#define ELFDATANONE     0               /* EI_DATA */
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2
#define ELFDATANUM      3

#define ET_NONE         0               /* e_type */
#define ET_REL          1
#define ET_EXEC         2
#define ET_DYN          3
#define ET_CORE         4
#define ET_NUM          5

#define ET_LOPROC       0xff00          /* processor specific range */
#define ET_HIPROC       0xffff

#define EM_NONE         0               /* e_machine */
#define EM_M32          1               /* AT&T WE 32100 */
#define EM_SPARC        2               /* Sun SPARC */
#define EM_386          3               /* Intel 80386 */
#define EM_68K          4               /* Motorola 68000 */
#define EM_88K          5               /* Motorola 88000 */
#define EM_486          6               /* Intel 80486 */
#define EM_860          7               /* Intel i860 */
#define EM_MIPS         8               /* Mips R2000 */
#define EM_S370         9               /* Amdhal       */
#define EM_NUM          10


#define EV_NONE         0               /* e_version, EI_VERSION */
#define EV_CURRENT      1
#define EV_NUM          2


/*
 * e_flags
 */

#define EF_MIPS_NOREORDER       0x00000001
#define EF_MIPS_OPSEX           EF_MIPS_NOREORDER
#define EF_MIPS_PIC             0x00000002
#define EF_MIPS_CPIC            0x00000004
#define EF_MIPS_XGOT            0x00000008
#define EF_MIPS_64BIT_WHIRL     0x00000010
#define EF_MIPS_ABI2            0x00000020 
#define EF_MIPS_ARCH            0xf0000000
#define EF_MIPS_ARCH_1          0x00000000
#define EF_MIPS_ARCH_2          0x10000000
#define EF_MIPS_ARCH_3          0x20000000
#define EF_MIPS_ARCH_4          0x30000000


#define PT_NULL         0               /* p_type */
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6
#define PT_NUM          7

#define PT_LOPROC       0x70000000      /* processor specific range */
#define PT_HIPROC       0x7fffffff

#define PF_R            0x4             /* p_flags */
#define PF_W            0x2
#define PF_X            0x1

#define SHT_NULL        0               /* sh_type */
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11
#define SHT_NUM         12
#define SHT_LOUSER      0x80000000
#define SHT_HIUSER      0xffffffff

#define SHT_LOPROC      0x70000000      /* processor specific range */
#define SHT_HIPROC      0x7fffffff

#define SHF_WRITE       0x1             /* sh_flags */
#define SHF_ALLOC       0x2
#define SHF_EXECINSTR   0x4

#define SHF_MASKPROC    0xf0000000      /* processor specific values */

#define SHN_UNDEF       0               /* special section numbers */
#define SHN_LORESERVE   0xff00
#define SHN_ABS         0xfff1
#define SHN_COMMON      0xfff2
#define SHN_HIRESERVE   0xffff

#define SHN_LOPROC      0xff00          /* processor specific range */
#define SHN_HIPROC      0xff1f

#endif 
