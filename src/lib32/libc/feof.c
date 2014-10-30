/* $Id: feof.c,v 1.3 1999/03/08 07:33:43 gernot Exp $ */
#include <termio.h>

/*************************************************************
 *  feof(fp)
 *      Test a file to see if it's at EOF
 */
feof (fp)
     FILE           *fp;
{
    int             fd;
    Ramfile        *p;

    fd = fp->fd;
    if (fd < FILEOFFSET)
	return (0);

    fd -= FILEOFFSET;
    if (_mfile[fd].open != 1)
	return (0);

    p = &_mfile[fd];
    if (p->posn >= p->size)
	return (1);
    return (0);
}
