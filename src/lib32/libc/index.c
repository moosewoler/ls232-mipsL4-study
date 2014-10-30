/* $Id: index.c,v 1.3 1999/03/08 07:33:53 gernot Exp $ */
#include "string.h"

/** char *index(s,c) returns ptr to 1st c in s, else 0 */
char           *
index (s, c)
     char           *s;
     int             c;
{

    if (s == 0)
	return (0);

    for (; *s; s++) {
	if (*s == c)
	    return (s);
    }
    return (0);
}
