/* $Id: strncmp.c,v 1.3 1999/03/08 07:34:31 gernot Exp $ */
#include "string.h"

/** int strncmp(s1,s2,n) as strcmp, but compares at most n characters */
int 
strncmp (s1, s2, n)
     char           *s1, *s2;
     int             n;
{

    if (!s1 || !s2)
	return (0);

    while (n && (*s1 == *s2)) {
	if (*s1 == 0)
	  return (0);
	s1++;
	s2++;
	n--;
    }
    if (n)
      return (*s1 - *s2);
    return (0);
}
