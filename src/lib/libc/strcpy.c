#include "string.h"

/** char *strcpy(dst,src) copy src to dst */
char *strcpy (char *dstp, const char *srcp)
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
