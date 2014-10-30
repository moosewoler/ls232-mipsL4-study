/* $Id: strlen.c,v 1.3 1999/03/08 07:34:29 gernot Exp $ */
#include "string.h"

/** int strlen(p) returns the length of p */
int 
strlen (p)
     char           *p;
{
    int             n;

    if (!p)
	return (0);
    for (n = 0; *p; p++)
	n++;
    return (n);
}
