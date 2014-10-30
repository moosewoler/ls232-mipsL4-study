/* $Id: isxdigit.c,v 1.3 1999/03/08 07:34:01 gernot Exp $ */
/** isxdigit(c) returns true if c is a hex digit */
isxdigit (c)
     int             c;
{
    if (c >= '0' && c <= '9')
	return (1);
    if (c >= 'a' && c <= 'f')
	return (1);
    if (c >= 'A' && c <= 'F')
	return (1);
    return (0);
}
