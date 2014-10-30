#ifndef _UNSWR4600
#define _UNSWR4600
/****************************************************************************
 * $Id: u4600.h,v 1.5 1998/01/22 05:46:14 kevine Exp $
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
 * MEMORY MAP for UNSW R4600 Board
 ****************************************************************************/


/* default DECODE */
/* DRAM banks */

#define DRAM10_BASE    0x00000000
#define DRAM32_BASE    0x01000000
#define DRAM_END       0x02000000

/* Device Banks */
#define CS210_BASE    0x1c000000
#define CS210_END     0x1e200000
#define CS3B_BASE     0x1f000000
#define CS3B_END      0x20000000

/* PCI decode */
#define PCI_IO_BASE   0x10000000
#define PCI_IO_END    0x12000000
#define PCI_MEM_BASE  0x12000000
#define PCI_MEM_END   0x14000000

#define PCI_MEM_SPACE PCI_MEM_BASE
#define PCI_IO_SPACE  PCI_IO_BASE
#define PCI_MEM_SPACE_SIZE	(32 * 1024 * 1024)
#define PCI_IO_SPACE_SIZE	(32  * 1024 * 1024)


/* DRAM and DEVICE ADDRESS MAP */
#define DRAM0_BASE     0x00000000
#define DRAM1_BASE     0x00800000
#define DRAM2_BASE     0x01000000
#define DRAM3_BASE     0x01800000

#define CS0_BASE      0x1c000000
#define CS1_BASE      0x1c800000
#define CS2_BASE      0x1d000000
#define CS3_BASE      0x1f000000
#define BOOT_BASE     0x1fc00000

/* PERIPHERAL Address map */
#define RTCLOCK_BASE   CS0_BASE

#define LED_BASE    (CS1_BASE)
#define IO_PINS_BASE (CS1_BASE | 0x10)
#define PCI_INT_BASE (CS1_BASE | 0x20)
#define Z85230_BASE (CS1_BASE | 0x30)


/* interrupt controller address map */

#define PCI_INT_IPLO    (PCI_INT_BASE | 0)
#define PCI_INT_IPHI    (PCI_INT_BASE | 1)
#define PCI_INT_IMLO    (PCI_INT_BASE | 2)
#define PCI_INT_IMHI    (PCI_INT_BASE | 3)

#if defined(_LANGUAGE_C)
typedef struct {
  volatile unsigned char ip_lo;
  volatile unsigned char ip_hi;
  volatile unsigned char im_lo;
  volatile unsigned char im_hi;
} pic_t;

#endif

/* TIMING for BOOT BANK with M27C4002-10 */

#define EPROM_TurnOff     3
#define EPROM_AccToFirst  5
#define EPROM_AccToNext   5

/* TIMING for BANK 0 with M48T02-150 */
#define M48_TurnOff       3 /* GT minimum */
#define M48_AccToFirst    8
#define M48_AccToNext     8
#define M48_ADStoWr       3  
#define M48_WrActive      5
#define M48_WrHigh        3

   
/* nvram layout (to do memory sizing) */
#define NVOFFSET	0		/* use all of NVRAM */
#define NVOFF_BANK0  	(NVOFFSET+16)   /* 4 size of ram RAM */
#define NVOFF_BANK1  	(NVOFFSET+20)   /* 4 size of ram RAM */
#define NVOFF_BANK2  	(NVOFFSET+24)   /* 4 size of ram RAM */
#define NVOFF_BANK3  	(NVOFFSET+28)   /* 4 size of ram RAM */

/* CPU interrupt assignments in l4 'intr id + 1' format */
#define INT_IO_CON      1               /* CPU pin int0 */
#define INT_PIC         2               /* CPU pin int1 */
#define INT_Z85230      3               /* CPU pin int2 */
#define INT_GT64010A    4               /* CPU pin int3 */
#define INT_SW_DEBUG    5               /* CPU pin int4 */

#endif
