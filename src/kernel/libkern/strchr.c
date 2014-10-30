#include "string.h"

/** char *strchr(p,c) return a ptr to c in p, return NUL if not found */
char * strchr (char *p, char c)
{

    if (!p)
	return (0);

    for (; *p; p++)
	if (*p == c)
	    return (p);
    return (0);
}
