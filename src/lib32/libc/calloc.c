/* $Id: calloc.c,v 1.3 1999/03/08 07:33:35 gernot Exp $ */
#include <malloc.h>

char           *
calloc (nelem, elsize)
     unsigned        nelem, elsize;
{
    char           *p;

    p = malloc (nelem * elsize);
    if (p == 0)
	return (p);

    bzero (p, nelem * elsize);
    return (p);
}
