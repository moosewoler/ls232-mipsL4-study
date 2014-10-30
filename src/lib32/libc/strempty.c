/* $Id: strempty.c,v 1.3 1999/03/08 07:34:27 gernot Exp $ */
#include "string.h"

/** strempty(p) returns 1 if p contains nothing but isspace */
strempty (p)
     char           *p;
{

    if (!p)
	return (1);
    for (; *p; p++)
	if (!isspace (*p))
	    return (0);
    return (1);
}
