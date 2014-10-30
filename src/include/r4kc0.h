#ifndef R4KC0_H
#define R4KC0_H
/****************************************************************************
 * $Id: r4kc0.h,v 1.18 1998/05/14 07:47:24 kevine Exp $
 * Copyright (C) 1997, 1998 Kevin Elphinstone, Univeristy of New South
 * Wales.
 *
 * This file is part of the L4/MIPS micro-kernel distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ****************************************************************************/

/****************************************************************************
 * Coprocessor 0 and Memory Management defines sources from MIPS R4000
 * Microprocessor User's Manual.
 *
 ****************************************************************************/

/* number of ASIDs on the R4x00 */

#define R4K_MAX_ASID 255 /* 255 */

/* 32-bit Kernel Memory Map */

#define KSEG0       0x80000000
#define KSEG1       0xa0000000
#define KSSEG       0xc0000000
#define KSEG3       0xe0000000
#define KUSEG       0x00000000

#define PHYS_TO_KSEG1(n)    (KSEG1 | (n))
#define PHYS_TO_KSEG0(n)    (KSEG0 | (n))

/* 64-bit kernel memory map */

#define MAXUSEG     0x0000010000000000

#define XKUSEG      0x0000000000000000
#define XKSSEG      0x4000000000000000
#define XKPHYS      0x8000000000000000

/* caching attributes for XKPHYS (source R4600 Hardware Users Manual) */
#define XKPHYS0     0x8000000000000000 /* Cacheable, non-coherent,
				        * write-through, no write-allocate
				        */
#define XKPHYS1     0x8800000000000000 /* Cacheable, non-coherent,
					* write-through, write-allocate
					*/
#define XKPHYS2     0x9000000000000000 /* Uncached */
#define XKPHYS3     0x9800000000000000 /* Cacheable, non-coherent */

#define XKSEG       0xc000000000000000
#define CKSEG0      0xffffffff80000000
#define CKSEG1      0xffffffffa0000000
#define CKSSEG      0xffffffffc0000000
#define CKSEG3      0xffffffffe0000000

/* exception vector locations */
#define VECTOR_BASE CKSEG1  /* actually CKSEG0, but use uncached region */
#define VECTOR_BASE_BEV 0xffffffffbfc00200

#define TLB_OFFSET       0x000
#define XTLB_OFFSET      0x080
#define CACHE_ERR_OFFSET 0x100
#define GEN_EXCPT_OFFSET 0x180

#define TLB_VEC          (VECTOR_BASE | TLB_OFFSET)
#define XTLB_VEC         (VECTOR_BASE | XTLB_OFFSET)
#define CACHE_ERR_VEC    (VECTOR_BASE | CACHE_ERR_OFFSET)
#define GEN_EXCPT_VEC    (VECTOR_BASE | GEN_EXCPT_OFFSET)

#define PHYS_TO_XKPHYS2(n)    (XKPHYS2 | (n))
#define PHYS_TO_CKSEG0(n)     (CKSEG0 | (n))
#define PHYS_TO_CKSEG1(n)     (CKSEG1 | (n))



#define CKSEG1_TO_PHYS(n)     ((n) & (CKSSEG-CKSEG1-1))
#define CKSEG0_TO_PHYS(n)     ((n) & (CKSEG1-CKSEG0-1))

#ifdef _LANGUAGE_ASSEMBLY

/* coprocessor 0 registers */
#define C0_INDEX         $0
#define C0_RANDOM        $1
#define C0_ENTRYLO0      $2
#define C0_ENTRYLO1      $3
#define C0_PAGEMASK      $5
#define C0_WIRED         $6
#define C0_BADVADDR      $8
#define C0_COUNT         $9
#define C0_ENTRYHI       $10
#define C0_COMPARE       $11
#define C0_STATUS        $12
#define C0_CAUSE         $13
#define C0_EPC           $14
#define C0_PRID          $15
#define C0_CONFIG        $16
#define C0_XCONTEXT      $20
#define C0_TAGLO         $28
#define C0_TAGHI         $29
#endif

/* status register fields */
#define ST_IE            0x0001
#define ST_EXL           0x0002
#define ST_ERL           0x0004
#define ST_KSU           0x0018
#define ST_KSU_U         0x0010
#define ST_UX            0x0020
#define ST_SX            0x0040
#define ST_KX            0x0080
#define ST_IM0           0x0100
#define ST_IM1           0x0200
#define ST_IM2           0x0400
#define ST_IM3           0x0800
#define ST_IM4           0x1000
#define ST_IM5           0x2000
#define ST_IM6           0x4000
#define ST_IM7           0x8000

#define ST_IM            0xff00

#define ST_DE        0x00010000
#define ST_CE        0x00020000
#define ST_CH        0x00040000
/* zero */
#define ST_SR        0x00100000
/* zero */
#define ST_BEV       0x00400000
/* zero */
/* zero */
#define ST_RE        0x02000000
#define ST_FR        0x04000000
/* zero */
#define ST_CU0       0x10000000
#define ST_CU1       0x20000000
#define ST_CU2       0x40000000
#define ST_CU3       0x80000000

/* cause register fields */

#define CA_EXC_CODE  0x0000007c
#define CA_Int        0
#define CA_Mod       (1 << 2)
#define CA_TLBL      (2 << 2)
#define CA_TLBS      (3 << 2)
#define CA_AdEL      (4 << 2)
#define CA_AdES      (5 << 2)
#define CA_IBE       (6 << 2)
#define CA_DBE       (7 << 2)
#define CA_Sys       (8 << 2)
#define CA_Bp        (9 << 2)
#define CA_RI        (10 << 2)
#define CA_CpU       (11 << 2)
#define CA_Ov        (12 << 2)
#define CA_Tr        (13 << 2)
#define CA_FPE       (15 << 2)


#define CA_IP0       0x00000100
#define CA_IP1       0x00000200
#define CA_IP2       0x00000400
#define CA_IP3       0x00000800
#define CA_IP4       0x00001000
#define CA_IP5       0x00002000
#define CA_IP6       0x00004000
#define CA_IP7       0x00008000

#define CA_CE_MASK   0x30000000
#define CA_CE_FP     0x10000000

/* EntryLo Registers */

#define EL_Global     0x01
#define EL_Valid      0x02
#define EL_Dirty      0x04
#define EL_Cache      0x38

#ifndef NO_CACHING
#define EL_Cache_CNWt    0x00    /* cacheable, noncoherent,
				    write through, no write allocate */
#define EL_Cache_CNWtWa (1 << 3) /* cacheable, noncoherent,
				    write through, write allocate */
#define EL_Cache_UC     (2 << 3) /* uncached */
#define EL_Cache_CNWb   (3 << 3) /* cacheable. noncoherent, write back */
#else
/* turn off caching for debugging with logic analyser */
#define EL_Cache_CNWb   (2 << 3)
#define EL_Cache_CNWtWa (2 << 3) 
#define EL_Cache_UC     (2 << 3)
#define EL_Cache_CNWt   (2 << 3)
#endif

#define EL_PFN           07777777700

/* CACHE intruction constants */
#define I_INDEX_INV                   0
#define D_INDEX_WB_INV                1
#define I_INDEX_LD_TAG                4
#define D_INDEX_LD_TAG                5
#define I_INDEX_ST_TAG                8
#define D_INDEX_ST_TAG                9
#define D_CRT_DRTY_XCL               13
#define I_HIT_INV                    16
#define D_HIT_INV                    17
#define D_HIT_WB_INV                 21
#define I_FILL                       20
#define D_HIT_WB                     25
#define I_HIT_WB                     24

/* EntryHi bit definitions */
#define EH_ASID_MASK                 0377
#endif
