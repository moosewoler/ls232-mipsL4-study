/* $Id: strbalp.c,v 1.3 1999/03/08 07:34:22 gernot Exp $ */
#include "string.h"

/** char *strbalp(p) return a ptr to balancing paren */
char           *
strbalp (p)
     char           *p;
{
    char            b, e;
    char           *ol = "({[<";
    char           *cl = ")}]>";
    int             i, n;

    b = *p;
    for (i = 0; ol[i] != 0; i++) {
	if (ol[i] == b)
	    break;
    }
    if (ol[i] == 0)
	return (0);
    e = cl[i];

    n = 0;
    for (; *p; p++) {
	if (*p == b)
	    n++;
	else if (*p == e) {
	    n--;
	    if (n == 0)
		return (p);
	}
    }
    return (0);
}
