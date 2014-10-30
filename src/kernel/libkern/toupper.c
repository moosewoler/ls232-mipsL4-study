#include "string.h"
#include "ctype.h"

/** toupper(c) translate c to uppercase */
int toupper (int c)
{

    if (islower (c))
	return (c - ('a' - 'A'));
    return (c);
}
