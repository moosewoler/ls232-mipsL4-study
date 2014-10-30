/* $Id: strrpset.c,v 1.3 1999/03/08 07:34:35 gernot Exp $ */
#include "string.h"

/** char *strrpset(str,set) like strrset except ignores inner parens */
char           *
strrpset (str, set)
     char           *str, *set;
{
    int             n;
    char           *p;

    n = 0;
    for (p = &str[strlen (str) - 1]; p > str; p--) {
	if (*p == '(')
	    n++;
	else if (*p == ')')
	    n--;
	else if (strchr (set, *p) && n == 0)
	    return (p);
    }
    return (0);
}
