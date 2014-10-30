/* $Id: isdigit.c,v 1.3 1999/03/08 07:33:56 gernot Exp $ */
/** isdigit(c) returns true if c is decimal digit */
isdigit (c)
     int             c;
{
    if (c >= '0' && c <= '9')
	return (1);
    return (0);
}
