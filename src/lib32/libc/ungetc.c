/* $Id: ungetc.c,v 1.3 1999/03/08 07:34:45 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  ungetc(fp) unget a char
 */
ungetc (c, fp)
     int             c;
     FILE           *fp;
{

    fp->ungetcflag = 1;
    fp->ungetchar = c;
    return (c);
}
