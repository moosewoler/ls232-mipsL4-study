/* $Id: lseek.c,v 1.3 1999/03/08 07:34:03 gernot Exp $ */
#include <termio.h>
#include <errno.h>
#include <unistd.h>

/*************************************************************
 *  lseek(fd,offset,whence)
 */
lseek (fd, offset, whence)
     int             fd, whence;
     long            offset;
{
    Ramfile        *p;

    if (fd < FILEOFFSET) {
	/* can't seek on tty devices */
	errno = EINVAL;
	return (-1);
    }
    fd -= FILEOFFSET;
    if (_mfile[fd].open != 1) {
	errno = EBADF;
	return (-1);
    }
    p = &_mfile[fd];
    switch (whence) {
    case SEEK_SET:
	p->posn = offset;
	break;
    case SEEK_CUR:
	p->posn += offset;
	break;
    case SEEK_END:
	p->posn = p->size + offset;
	break;
    default:
	errno = EINVAL;
	return (-1);
    }
    return (p->posn);
}
