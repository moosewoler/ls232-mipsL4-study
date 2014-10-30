#ifndef _COFF_H
#define _COFF_H
/****************************************************************************
 *      $Id: coff.h,v 1.5 1998/09/03 06:55:53 alanau Exp $
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

/* file hdr */

struct filehdr {
        unsigned short  f_magic;        /* magic number */
        unsigned short  f_nscns;        /* number of sections */
        int            f_timdat;       /* time & date stamp */
        int            f_symptr;       /* file pointer to symbolic header */
        int            f_nsyms;        /* sizeof(symbolic hdr) */
        unsigned short  f_opthdr;       /* sizeof(optional hdr) */
        unsigned short  f_flags;        /* flags */
        };

#define FILHDR  struct filehdr
#define FILHSZ  sizeof(FILHDR)
#define  MIPSEBMAGIC    0x0160
#define  SMIPSEBMAGIC   0x6001

/* aouthdr */
#define OMAGIC  0407
#define NMAGIC  0410
#define ZMAGIC  0413

typedef	struct aouthdr {
	short	magic;		/* see above				*/
	short	vstamp;		/* version stamp			*/
	int	tsize;		/* text size in bytes, padded to DW bdry*/
	int	dsize;		/* initialized data "  "		*/
	int	bsize;		/* uninitialized data "   "		*/
	int	entry;		/* entry pt.				*/
	int	text_start;	/* base of text used for this file	*/
	int	data_start;	/* base of data used for this file	*/
	int	bss_start;	/* base of bss used for this file	*/
	int	gprmask;	/* general purpose register mask	*/
	int	cprmask[4];	/* co-processor register masks		*/
	int	gp_value;	/* the gp value used for this object    */
} AOUTHDR;

#define AOUTHSZ sizeof(AOUTHDR)

struct scnhdr {
	char	
	s_name[8];	/* section name */
	int		s_paddr;	/* physical address, aliased s_nlib */
	int		s_vaddr;	/* virtual address */
	int		s_size;		/* section size */
	int		s_scnptr;	/* file ptr to raw data for section */
	int		s_relptr;	/* file ptr to relocation */
	int		s_lnnoptr;	/* file ptr to gp histogram */
	unsigned short	s_nreloc;	/* number of relocation entries */
	unsigned short	s_nlnno;	/* number of gp histogram entries */
	int		s_flags;	/* flags */
	};

#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)
#define STYP_REG        0x00000000      /* "regular" section:
                                                allocated, relocated, loaded */
#define STYP_DSECT      0x00000001      /* "dummy" section:
                                                not allocated, relocated,
                                                not loaded */
#define STYP_NOLOAD     0x00000002      /* "noload" section:
                                                allocated, relocated,
                                                 not loaded */
#define STYP_GROUP      0x00000004      /* "grouped" section:
                                                formed of input sections */
#define STYP_PAD        0x00000008      /* "padding" section:
                                                not allocated, not relocated,
                                                 loaded */
#define STYP_COPY       0x00000010      /* "copy" section:
                                                for decision function used
                                                by field update;  not
                                                allocated, not relocated,
                                                loaded;  reloc & lineno
                                                entries processed normally */
#define STYP_TEXT       0x00000020      /* section contains text only */
#define STYP_DATA       0x00000040      /* section contains data only */
#define STYP_BSS        0x00000080      /* section contains bss only */
#define STYP_RDATA      0x00000100      /* section contains read only data */
#define STYP_SDATA      0x00000200      /* section contains small data only */
#define STYP_SBSS       0x00000400      /* section contains small bss only */
#define STYP_UCODE      0x00000800      /* section only contains ucodes */
#define STYP_LIT8       0x08000000      /* literal pool for 8 byte literals */
#define STYP_LIT4       0x10000000      /* literal pool for 4 byte literals */

#endif
