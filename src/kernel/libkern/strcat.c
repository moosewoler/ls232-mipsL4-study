#include "string.h"

/** char *strcat(dst,src)  concatinate src string to dst string */
char *strcat (char *dst, char *src)
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
