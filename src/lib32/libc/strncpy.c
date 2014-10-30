/* $Id: strncpy.c,v 1.3 1999/03/08 07:34:31 gernot Exp $ */
#include "string.h"

/** char *strncpy(dst,src,n) copy n chars from src to dst */
char           *
strncpy (dst, src, n)
     char           *dst, *src;
     int             n;
{
    char           *d;

    if (!dst || !src)
	return (dst);
    d = dst;
    for (; *src && n; d++, src++, n--)
	*d = *src;
    while (n--)
	*d++ = '\0';
    return (dst);
}
