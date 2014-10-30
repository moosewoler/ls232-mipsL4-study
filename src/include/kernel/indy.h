#ifndef INDY_H
#define INDY_H
/****************************************************************************
 * $Id: indy.h,v 1.2 1998/01/22 05:46:05 kevine Exp $
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
 * Random SGI Indy addresses gleaned from various header files
 *
 ****************************************************************************/
#define IP22BOFF(X)		((X)|0x3)

/* the z85230 serial chip */
#define HPC3_SERIAL1_CMD		0x1fbd9830	/* 6,0x0c */
#define HPC3_SERIAL1_DATA		0x1fbd9834	/* 6,0x0d */

/* not sure, but header file has serial macro that reads from here after
   each write to serial chip */
#define HPC3_INTSTAT_ADDR	0x1fbb0000 

/* Addresses for the 8254 timer */
#define HPC3_INT3_ADDR          0x1fbd9880      /* 6,0x20 XXX - IP24 */
#define HPC3_INT2_ADDR		0x1fbd9000
#define HPC3_INT_ADDR		HPC3_INT3_ADDR

#define	PT_CLOCK_OFFSET         0x30
#define	PT_CLOCK_ADDR           (HPC3_INT_ADDR+PT_CLOCK_OFFSET)
#define MASTER_FREQ     1000000         /* master frequency */

/* clear timer 2 bits (ws) */
#define TIMER_ACK_ADDR          (HPC3_INT_ADDR+TIMER_ACK_OFFSET)
#define TIMER_ACK_OFFSET        IP22BOFF(0x20)
#define ACK_TIMER0      0x1     /* write strobe to clear timer 0 */
#define ACK_TIMER1      0x2     /* write strobe to clear timer 1 */



#ifndef LANGUAGE_ASSEMBLY  /* included in some standalone assembler files */

struct pt_clock {
        char fill0[3];
        unsigned char   pt_counter0;            /* counter 0 port */
        char fill1[3];
        unsigned char   pt_counter1;            /* counter 1 port */
        char fill2[3];
        unsigned char   pt_counter2;            /* counter 2 port */
        char fill3[3];
        unsigned char   pt_control;             /* control word */
};
#else  /* LANGUAGE_ASSEMBLY */

#define PT_COUNTER0    3               /* assembly structure offsets */
#define PT_COUNTER1    7
#define PT_COUNTER2    11
#define PT_CONTROL     15

#endif /* LANGUAGE_ASSEMBLY */

/* clear timer 2 bits (ws) */
#define	TIMER_ACK_ADDR		(HPC3_INT_ADDR+TIMER_ACK_OFFSET)
#define	TIMER_ACK_OFFSET	IP22BOFF(0x20)
#define ACK_TIMER0	0x1	/* write strobe to clear timer 0 */
#define ACK_TIMER1	0x2	/* write strobe to clear timer 1 */

#endif
