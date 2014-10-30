/* $Id: strrrot.c,v 1.3 1999/03/08 07:34:35 gernot Exp $ */

char           *
strrrot (p)
     char           *p;
{
    int             n, t;

    n = strlen (p);
    if (n < 2)
	return (p);

    t = p[--n];
    for (--n; n >= 0; n--)
	p[n + 1] = p[n];
    *p = t;
    return (p);
}
