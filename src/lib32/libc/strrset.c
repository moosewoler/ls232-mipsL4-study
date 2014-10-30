/* $Id: strrset.c,v 1.3 1999/03/08 07:34:36 gernot Exp $ */
#include "string.h"

/** char *strrset(p,set) returns a ptr to last char from set in p, else 0 */
char           *
strrset (p, set)
     char           *p, *set;
{
    int             i;

    for (i = strlen (p); i >= 0; i--) {
	if (strchr (set, p[i]))
	    return (&p[i]);
    }
    return (0);
}
