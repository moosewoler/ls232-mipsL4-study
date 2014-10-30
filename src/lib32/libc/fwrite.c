/* $Id: fwrite.c,v 1.3 1999/03/08 07:33:49 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  fwrite(dst,size,count,fp)
 */
fwrite (dst, size, count, fp)
     const void     *dst;
     int             size, count;
     FILE           *fp;
{
    int             n;

    n = write (fp->fd, dst, size * count);
    return (n / size);
}
