/* $Id: fprintf.c,v 1.3 1999/03/08 07:33:46 gernot Exp $ */
#include <stdarg.h>
#include <stdio.h>

/*************************************************************
 *  fprintf(fp,fmt,va_alist) send formatted string to stream
 */

int 
fprintf (FILE *fp, const char *fmt, ...)
{
    va_list         ap;
    int             len;

    va_start (ap, fmt);
    len = vfprintf (fp, fmt, ap);
    va_end (ap);
    return (len);
}
