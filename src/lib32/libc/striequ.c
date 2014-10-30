/* $Id: striequ.c,v 1.3 1999/03/08 07:34:28 gernot Exp $ */
#include "string.h"

/** int striequ(s1,s2) returns 1 if s1 matches s2 ignoring case, else 0 */
int 
striequ (s1, s2)
     char           *s1, *s2;
{

    if (!s1 || !s2)
	return (0);
    for (; *s1; s1++, s2++) {
	if (toupper (*s1) != toupper (*s2))
	    return (0);
    }
    if (*s2)
	return (0);
    return (1);
}
