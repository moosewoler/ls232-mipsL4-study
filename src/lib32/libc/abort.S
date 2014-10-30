/* $Id: abort.S,v 1.3 1999/03/08 07:33:27 gernot Exp $ */
#include <regdef.h>

	.globl abort
	.ent abort
abort:
	break	0
	j	ra
	.end abort
