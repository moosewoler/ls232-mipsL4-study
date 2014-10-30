#ifndef P4000I_BOARD_H
#define P4000I_BOARD_H
/****************************************************************************
 * $Id: p4000i.h,v 1.2 1998/01/22 05:46:10 kevine Exp $
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
 *
 * Algorithmics P4000i board defs sourced from p-4000i User's Manual
 *
 ****************************************************************************/



/* device address space */
#define RESET_BASE            0x1f100000
#define ALPHN_BASE            0x1f200000


/* Alphanumeric Display */
#define ALPHN_CLEAR           (RESET_BASE)
#define ALPHN_CURSEN          (RESET_BASE+4)
#define ALPHN_BLANK           (RESET_BASE+8)

#define ALPHN_CHAR_BASE       (ALPHN_BASE+16)
#define ALPHN_CURS_BASE       (ALPHN_BASE)

#define ALPHN_CURS(n)         (((3 - (n)) << 2) + ALPHN_CURS_BASE)
#define ALPHN_CHAR(n)         (((3 - (n)) << 2) + ALPHN_CHAR_BASE)

/* serial port */
#define SIO_RESET             (RESET_BASE + 12)

/* SONIC ethernet controller */
#define NET_RESET             (RESET_BASE + 16)

/* interrupt registers */
#define IRR_LO               0x1f700000
#define IRR_HI               0x1f700020
#define IMR0                 0x1f700000
#define IMR1                 0x1f700004
#define IMR2                 0x1f700008
#define IMR3                 0x1f70000c
#define IPANIC               0x1f700040
#define IGC0                 0x1f700020
#define IGC1                 0x1f700024
#define IGC2                 0x1f700028
#define IGC3                 0x1f70002c


#endif
