/* $Id: strbequ.c,v 1.3 1999/03/08 07:34:22 gernot Exp $ */
#include "string.h"

/** int strbequ(s1,s2) return 1 if s2 matches 1st part of s1 */
int 
strbequ (s1, s2)
     char           *s1, *s2;
{

    if (!s1 || !s2)
	return (0);
    for (; *s1 && *s2; s1++, s2++)
	if (*s1 != *s2)
	    return (0);
    if (!*s2)
	return (1);
    return (0);
}
