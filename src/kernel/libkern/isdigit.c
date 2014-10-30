#include "ctype.h"

/** isdigit(c) returns true if c is decimal digit */
int isdigit (int c)
{
    if (c >= '0' && c <= '9')
	return (1);
    return (0);
}
