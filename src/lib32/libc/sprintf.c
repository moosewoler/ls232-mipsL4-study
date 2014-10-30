/* $Id: sprintf.c,v 1.3 1999/03/08 07:34:19 gernot Exp $ */
#include <stdarg.h>

/*************************************************************
 *  sprintf(buf,fmt,va_alist) send formatted string to buf
 */
int 
sprintf (char *buf, const char *fmt, ...)
{
    va_list         ap;
    int             n;

    va_start (ap, fmt);
    n = vsprintf (buf, fmt, ap);
    va_end (ap);
    return (n);
}
