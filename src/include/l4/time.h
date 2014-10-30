#ifndef __L4TIME_H__
#define __L4TIME_H__
/****************************************************************************
 * $Id: time.h,v 1.2 1999/02/10 04:55:05 gernot Exp $
 * Copyright (C) 1999, Gernot Heiser, University of New South
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
 * Functions for dealing with L4 timeout encodings
 ****************************************************************************/

#include <l4/types.h>


/****************************************************************************
 * Constants defining the direction of rounding when encoding times
 ****************************************************************************/

#define L4_T_ROUND_DOWN    0
#define L4_T_ROUND_NEAREST 1
#define L4_T_ROUND_UP      2


/****************************************************************************
 * Functions for encoding times
 *
 * Specifying L4_T_ROUND_NEAREST or L4_T_ROUND_DOWN will never result in
 * an encoding for infinity.
 * Specifying L4_T_ROUND_NEAREST or L4_T_ROUND_UP will never result in
 * an encoding for zero unless usecs==0.
 * Otherwise, L4_T_ROUND_NEAREST will round to the nearest representable time
 * value, i.e. will round UP if the value to be encoded is closer to or equally
 * far from the next larger representable value than the next smaller
 * representable value.
 ****************************************************************************/

void l4_mips_encode_timeout(dword_t msecs, byte_t *mant, byte_t *exp,
			    byte_t round);

void l4_mips_encode_pf_timeout(dword_t msecs, byte_t *exp, byte_t round);


/****************************************************************************
 * Functions for decoding times
 *
 * Functions return (~0l) for an infinite timeout,
 * otherwise the timeout value in milliseconds.
 ****************************************************************************/

dword_t l4_mips_decode_timeout(byte_t mant, byte_t exp);

dword_t l4_mips_decode_pf_timeout(byte_t exp);


/****************************************************************************
 * Sleep for a given time
 ****************************************************************************/

void l4_mips_sleep_msecs(dword_t msecs, byte_t round);

#endif
