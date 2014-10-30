/* $Id: strccat.c,v 1.3 1999/03/08 07:34:23 gernot Exp $ */
#include "string.h"

/** char *strccat(dst,c)  concatinate char to dst string */
char           *
strccat (dst, c)
     char           *dst, c;
{
    int             len;

    if (!dst)
	return (dst);
    len = strlen (dst);
    dst[len] = c;
    dst[len + 1] = 0;
    return (dst);
}
