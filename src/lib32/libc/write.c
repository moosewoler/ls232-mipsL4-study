/* $Id: write.c,v 1.3 1999/03/08 07:34:47 gernot Exp $ */
#include <termio.h>

write (fd, buf, n)
     int             fd, n;
     char           *buf;
{

    if (fd < FILEOFFSET)
	return (_write (fd, buf, n));

/* we don't support write to ram-based files */
    return (0);
}
