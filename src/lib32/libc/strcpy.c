/* $Id: strcpy.c,v 1.3 1999/03/08 07:34:25 gernot Exp $ */
#include "string.h"

/** char *strcpy(dst,src) copy src to dst */
char           *
strcpy (dstp, srcp)
     char           *dstp, *srcp;
{
    char           *dp = dstp;

    if (!dstp)
	return (0);
    *dp = 0;
    if (!srcp)
	return (dstp);

    while ((*dp++ = *srcp++) != 0);
    return (dstp);
}
