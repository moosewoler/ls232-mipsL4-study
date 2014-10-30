/* $Id: putchar.c,v 1.3 1999/03/08 07:34:09 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  putchar(c) put char to stdout
 */
putchar (c)
     int             c;
{
    putc (c, stdout);
}
