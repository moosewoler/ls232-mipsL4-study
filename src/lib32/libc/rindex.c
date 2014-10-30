/* $Id: rindex.c,v 1.3 1999/03/08 07:34:15 gernot Exp $ */
#include "string.h"

/** char *rindex(s,c) returns ptr to c in s, starting at end of s, else 0 */
char           *
rindex (s, c)
     char           *s;
     int             c;
{
    char           *p;

    if (s == 0)
	return (0);

    for (p = s; *p; p++);
    for (; p >= s; p--) {
	if (*p == c)
	    return (p);
    }
    return (0);
}
