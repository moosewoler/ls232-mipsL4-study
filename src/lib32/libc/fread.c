/* $Id: fread.c,v 1.3 1999/03/08 07:33:48 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  fread(src,size,count,fp)
 */
fread (src, size, count, fp)
     void           *src;
     int             size, count;
     FILE           *fp;
{
    int             n;

    n = read (fp->fd, src, size * count);
    return (n / size);
}
