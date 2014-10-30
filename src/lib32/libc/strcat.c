/* $Id: strcat.c,v 1.3 1999/03/08 07:34:23 gernot Exp $ */
#include "string.h"

/** char *strcat(dst,src)  concatinate src string to dst string */
char           *
strcat (dst, src)
     char           *dst, *src;
{
    char           *d;

    if (!dst || !src)
	return (dst);
    d = dst;
    for (; *d; d++);
    for (; *src; src++)
	*d++ = *src;
    *d = 0;
    return (dst);
}
