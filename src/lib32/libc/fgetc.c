/* $Id: fgetc.c,v 1.3 1999/03/08 07:33:44 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  fgetc(fp) get char from stream
 */
fgetc (fp)
     FILE           *fp;
{
    char            c;

    if (fp->ungetcflag) {
	fp->ungetcflag = 0;
	return (fp->ungetchar);
    }
    if (read (fp->fd, &c, 1) == 0)
	return (EOF);
    return (c);
}
