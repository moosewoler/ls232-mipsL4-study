#include "ctype.h"

/** islower(c) returns true if c is lower case */
int islower (int c)
{
    if (c >= 'a' && c <= 'z')
	return (1);
    return (0);
}


/** islower(c) returns true if c is lower case */
int isupper (int c)
{
    if (c >= 'A' && c <= 'Z')
	return (1);
    return (0);
}
