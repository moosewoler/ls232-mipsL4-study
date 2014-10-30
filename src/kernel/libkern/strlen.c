#include "string.h"

/** int strlen(p) returns the length of p */
size_t strlen (char *p)
{
    int             n;

    if (!p)
	return (0);
    for (n = 0; *p; p++)
	n++;
    return (n);
}
