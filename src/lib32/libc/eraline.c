/* $Id: eraline.c,v 1.3 1999/03/08 07:33:40 gernot Exp $ */
/*************************************************************
 *  era_line(s) erase line
 */
era_line (s)
     char           *s;
{
    while (*s++)
	printf ("\b \b");
}
