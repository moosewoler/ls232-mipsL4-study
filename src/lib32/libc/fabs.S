/* $Id: fabs.S,v 1.3 1999/03/08 07:33:42 gernot Exp $ */
#include "mips.h"

	.globl fabs
	.ent fabs
fabs:
	abs.d	$f0,$f12
	j	ra
	.end fabs
