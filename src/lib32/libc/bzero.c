/* $Id: bzero.c,v 1.3 1999/03/08 07:33:35 gernot Exp $ */
/** bzero(dst,length) clear length bytes in destination */
bzero (dst, length)
     char           *dst;
     int             length;
{
    memset (dst, 0, length);
}
