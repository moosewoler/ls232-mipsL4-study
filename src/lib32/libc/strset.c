/* $Id: strset.c,v 1.3 1999/03/08 07:34:37 gernot Exp $ */
#include "string.h"

/** char *strset(p,set) returns a ptr to 1st char from set in p, else 0 */
char           *
strset (p, set)
     char           *p, *set;
{

    for (; *p; p++) {
	if (strchr (set, *p))
	    return (p);
    }
    return (0);
}
