/* $Id: printf.c,v 1.3 1999/03/08 07:34:08 gernot Exp $ */
#include "stdarg.h"
#include "stdio.h"

extern FILE     _iob[OPEN_MAX];

int 
printf (const char *fmt, ...)
{
    va_list         ap;
    int             len;

    va_start (ap, fmt);
    len = vfprintf (stdout, fmt, ap);
    va_end (ap);
    return (len);
}
