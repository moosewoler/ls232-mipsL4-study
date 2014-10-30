#include <stdarg.h>
#include "stdio.h"

/*************************************************************
 *  sprintf(buf,fmt,va_alist) send formatted string to buf
 */

int 
sprintf (char *buf, const char *fmt, ...)
{
    va_list         ap;
    int             n;

    va_start (ap,fmt);
    n = vsprintf (buf, fmt, ap);
    va_end (ap);
    return (n);
}
