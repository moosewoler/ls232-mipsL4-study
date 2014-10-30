/* $Id: fseek.c,v 1.3 1999/03/08 07:33:48 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  fseek(fp,offset,whence)
 */
fseek (fp, offset, whence)
     FILE           *fp;
     long            offset;
     int             whence;
{
    int             n;

    fp->ungetcflag = 0;
    n = lseek (fp->fd, offset, whence);
    return (n);
}
