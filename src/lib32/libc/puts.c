/* $Id: puts.c,v 1.3 1999/03/08 07:34:10 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  puts(p) put string to stdout
 */
puts (p)
     const char     *p;
{

    fputs (p, stdout);
    putc ('\n', stdout);
}
