/* $Id: read.c,v 1.3 1999/03/08 07:34:14 gernot Exp $ */
#include <termio.h>

/** read(fd,buf,n) read n bytes into buf from fd */
read (fd, buf, n)
     int             fd, n;
     char           *buf;
{
    Ramfile        *p;

    if (fd < FILEOFFSET)
	return (_read (fd, buf, n));

    fd -= FILEOFFSET;
    if (_mfile[fd].open != 1)
	return (0);

    p = &_mfile[fd];
    if (p->posn + n > p->size)
	n = p->size - p->posn;
    memcpy (buf, (char *)p->base + p->posn, n);
    p->posn += n;
    return (n);
}
