/****************************************************************************
 * $Id: time.c,v 1.2 1999/02/10 04:56:56 gernot Exp $
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

#include <l4/time.h>
#include <l4/ipc.h>


void l4_mips_encode_timeout(dword_t msecs, byte_t *mant, byte_t *exp,
			    byte_t round) {
  dword_t ms;
  int trunc;

  *exp = 10;
  *mant = 0;
  if (msecs == 0  ||  (msecs == 1  &&  round == L4_T_ROUND_DOWN)) {
    return;
  }
  msecs = 1000*msecs;
  trunc = msecs%1024 != 0;
  ms    = ((msecs%1024)*4)/1024;
  ms    = ((msecs%1024)*4)/1024;
  msecs = msecs/1024;
  while (msecs >= 256  &&  *exp > 1) {
    (*exp)--;
    ms = msecs;
    if (msecs&3) {
      trunc=1;
    }
    msecs /= 4;
  }
  if (ms == 256) {
    *mant = msecs + (round == L4_T_ROUND_UP ? trunc : 0);
    return;
  } else if (*exp == 1) {
    if (round == L4_T_ROUND_UP  &&
	(msecs >= 256  ||  (msecs == 255 && trunc))) {
      *exp = 0;		/* INFINITY */
    } else if (msecs >= 255) {
      *mant = 255;	/* largest finite value */
    } else if ((msecs < 255 &&
		(trunc  &&  round == L4_T_ROUND_UP))  ||
		(ms&2   &&  round == L4_T_ROUND_NEAREST)) {
      *mant = msecs+1;
    } else {
      *mant = msecs;
    }
    return;
  }
  *mant = msecs;
  if (round == L4_T_ROUND_DOWN  ||
      (round == L4_T_ROUND_NEAREST  &&  !(ms&2))  ||
      (round == L4_T_ROUND_UP  &&  !trunc)) {
    return;
  } else if (msecs == 255) {
    *mant=64;
    (*exp)--;
  } else {
    (*mant)++;
    return;
  }
}


void l4_mips_encode_pf_timeout(dword_t msecs, byte_t *exp, byte_t round) {
  dword_t ms;
  int trunc;

  if (msecs == 0) {
    *exp = 15;
    return;
  }
  *exp = 10;	/* smallest non-zero timeout is 1ms */
  if (0 && msecs <= 4) {
    return;
  }
  trunc = 0;
  ms=4;
  while (msecs >= 4  && *exp > 1) {
    (*exp)--;
    if (msecs&3) {
      trunc=1;
    }
    ms = msecs;
    msecs /= 4;
  }
  if (round == L4_T_ROUND_DOWN  ||
      (round == L4_T_ROUND_UP  &&  !(msecs>1)  &&  !trunc)  ||
      (round == L4_T_ROUND_NEAREST  &&  (*exp == 1  ||  !(msecs&2)))) {
  } else if (round == L4_T_ROUND_UP  ||  msecs&2) {
    (*exp)--;
  }
  return;
}


/****************************************************************************
 * Functions for decoding times
 ****************************************************************************/


dword_t l4_mips_decode_timeout(byte_t mant, byte_t exp) {
  if (exp==0) {
    return ~(dword_t)0;
  } else if (mant==0) {
    return 0;
  } else {
    return ((mant * ((dword_t)1)<<(2*(15-exp)))+500) / 1000;
  }
}


dword_t l4_mips_decode_pf_timeout(byte_t exp) {
  if (exp==0) {
    return ~(dword_t)0;
  } else if (exp==15) {
    return 0;
  } else {
    return ((((dword_t)1)<<(2*(15-exp)))+500) / 1000;
  }
}


/****************************************************************************
 * Sleep for a given time
 ****************************************************************************/


void l4_mips_sleep_msecs(dword_t msecs, byte_t round) {
  byte_t exp, mant;
  l4_msgdope_t result;

  l4_mips_encode_timeout(msecs, &mant, &exp, round);
  l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0, 0, mant, exp, 0, 0), &result);
}
