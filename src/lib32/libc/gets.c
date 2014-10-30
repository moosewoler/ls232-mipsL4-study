/* $Id: gets.c,v 1.3 1999/03/08 07:33:52 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  char *gets(p) get line from stdin
 *      unlike fgets, a trailing \n gets deleted
 */
char           *
gets (p)
     char           *p;
{
    char           *s;
    int             n;


    s = fgets (p, MAXLN, stdin);
    if (s == 0)
	return (0);
    n = strlen (p);
    if (n && p[n - 1] == '\n')
	p[n - 1] = 0;
    return (s);
}
