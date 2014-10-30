/* $Id: strtoupp.c,v 1.3 1999/03/08 07:34:42 gernot Exp $ */
#include "string.h"

/** strtoupper(p) convert p to uppercase */
strtoupper (p)
     char           *p;
{
    if (!p)
	return;
    for (; *p; p++)
	*p = toupper (*p);
}
