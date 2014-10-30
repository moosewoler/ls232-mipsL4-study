/* $Id: strposn.c,v 1.3 1999/03/08 07:34:34 gernot Exp $ */
#include "string.h"

/** char *strposn(p,q) returns a ptr to q in p, else 0 if not found */
char           *
strposn (p, q)
     char           *p, *q;
{
    char           *s, *t;

    if (!p || !q)
	return (0);

    if (!*q)
	return (p + strlen (p));
    for (; *p; p++) {
	if (*p == *q) {
	    t = p;
	    s = q;
	    for (; *t; s++, t++) {
		if (*t != *s)
		    break;
	    }
	    if (!*s)
		return (p);
	}
    }
    return (0);
}
