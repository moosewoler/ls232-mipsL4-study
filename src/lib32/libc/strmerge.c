/* $Id: strmerge.c,v 1.3 1999/03/08 07:34:30 gernot Exp $ */
/************************************************************** 
 *  strmerge(d,s)
 */
strmerge (d, s)
     char           *d, *s;
{
    int             i;

    if (strlen (d) < strlen (s)) {
	for (i = strlen (d); i < strlen (s); i++)
	    d[i] = ' ';
	d[i] = 0;
    }
    for (; *d && *s; d++, s++)
	if (*s != ' ')
	    *d = *s;
}
