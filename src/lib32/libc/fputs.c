/* $Id: fputs.c,v 1.3 1999/03/08 07:33:47 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  fputs(string,fp) put string to stream
 */
fputs (p, fp)
     const char     *p;
     FILE           *fp;
{

    write (fp->fd, p, strlen (p));
}
