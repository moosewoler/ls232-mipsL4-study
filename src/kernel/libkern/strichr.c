#include "string.h"

/** char *strichr(p,c) inserts c as the first char of the string p */
char *strichr (char *p, int c)
{
    char           *t;

    if (!p)
	return (p);
    for (t = p; *t; t++);
    for (; t >= p; t--)
	*(t + 1) = *t;
    *p = c;
    return (p);
}
