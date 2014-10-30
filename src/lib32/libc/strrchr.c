/* $Id: strrchr.c,v 1.3 1999/03/08 07:34:34 gernot Exp $ */
#include "string.h"

/** char *strrchr(p,c) return a ptr to last c in p, return NUL if not found */
char           *
strrchr (p, c)
     char           *p, c;
{
    int             i;

    if (!p)
	return (0);

    for (i = strlen (p); i >= 0; i--)
	if (p[i] == c)
	    return (&p[i]);
    return (0);
}
