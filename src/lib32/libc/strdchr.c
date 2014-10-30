/* $Id: strdchr.c,v 1.3 1999/03/08 07:34:26 gernot Exp $ */
#include "string.h"

/** char *strdchr(p) deletes the first char from the string p */
char           *
strdchr (p)
     char           *p;
{
    char           *t;

    if (!p)
	return (p);
    for (t = p; *t; t++)
	*t = *(t + 1);
    return (p);
}
