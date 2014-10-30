/* $Id: strncat.c,v 1.3 1999/03/08 07:34:30 gernot Exp $ */
#include "string.h"

/** char *strncat(dst,src,n) concatinate n chars from src to dst */
char           *
strncat (dst, src, n)
     char           *dst, *src;
     int             n;
{
    char           *d;

    if (!dst || !src)
	return (dst);
    d = dst;
    for (; *d; d++);
    for (; *src && n; src++, n--)
	*d++ = *src;
    *d = 0;
    return (dst);
}
