/* $Id: strchr.c,v 1.3 1999/03/08 07:34:24 gernot Exp $ */
#include "string.h"

/** char *strchr(p,c) return a ptr to c in p, return NUL if not found */
char           *
strchr (p, c)
     char           *p, c;
{

    if (!p)
	return (0);

    for (; *p; p++)
	if (*p == c)
	    return (p);
    return (0);
}
