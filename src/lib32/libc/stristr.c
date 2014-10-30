/* $Id: stristr.c,v 1.3 1999/03/08 07:34:29 gernot Exp $ */
#include "string.h"

/** stristr(dst,p) insert string p into dst */
stristr (dst, p)
     char           *dst, *p;
{
    int             i;

    for (i = strlen (p); i > 0; i--)
	strichr (dst++, *p++);
}
