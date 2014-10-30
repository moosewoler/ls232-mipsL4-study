/* $Id: strnwrd.c,v 1.3 1999/03/08 07:34:32 gernot Exp $ */
#include "string.h"

/** strnwrd(p) returns a count of words in p */
strnwrd (p)
     char           *p;
{
    int             n;

    if (!p)
	return (0);

    for (n = 0; *p; n++) {
	while (isspace (*p))
	    p++;
	if (!*p)
	    return (n);
	while (!isspace (*p) && *p != 0)
	    p++;
    }
    return (n);
}
