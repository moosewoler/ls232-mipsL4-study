/* $Id: strspn.c,v 1.3 1999/03/08 07:34:41 gernot Exp $ */

/* return length of initial segment of p that consists entirely of
 * characters from s */

strspn (p, s)
     char           *p, *s;
{
    int             i, j;

    for (i = 0; p[i]; i++) {
	for (j = 0; s[j]; j++) {
	    if (s[j] == p[i])
		break;
	}
	if (!s[j])
	    break;
    }
    return (i);
}
