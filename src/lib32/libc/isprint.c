/* $Id: isprint.c,v 1.3 1999/03/08 07:34:00 gernot Exp $ */
/** isprint(c) returns true if c is a printing character */
isprint (c)
     int             c;
{
    if (c >= ' ' && c <= '~')
	return (1);
    return (0);
}
