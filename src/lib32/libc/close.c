/* $Id: close.c,v 1.3 1999/03/08 07:33:37 gernot Exp $ */
#include <termio.h>

/** close(fd) close fd */
close (fd)
     int             fd;
{

    if (fd < FILEOFFSET)
	return (_close (fd));

    fd -= FILEOFFSET;
    _mfile[fd].open = 0;
}
