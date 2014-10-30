#include "string.h"
#include "ctype.h"

/** strtoupper(p) convert p to uppercase */
void strtoupper (char *p)
{
    if (!p)
	return;
    for (; *p; p++)
	*p = toupper (*p);
}
