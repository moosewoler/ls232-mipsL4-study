/* $Id: toupper.c,v 1.3 1999/03/08 07:34:45 gernot Exp $ */
#include "string.h"

/** toupper(c) translate c to uppercase */
int 
toupper (c)
     int             c;
{

    if (islower (c))
	return (c - ('a' - 'A'));
    return (c);
}
