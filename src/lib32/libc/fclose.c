/* $Id: fclose.c,v 1.3 1999/03/08 07:33:42 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  fclose(fp) close stream
 */
fclose (fp)
     FILE           *fp;
{
    close (fp->fd);
    fp->valid = 0;
}
