/* $Id: putc.c,v 1.3 1999/03/08 07:34:09 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  putc(c,fp) put char to stream
 */
putc (c, fp)
     char            c;
     FILE           *fp;
{
    write (fp->fd, &c, 1);
}
