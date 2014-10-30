/* $Id: isalnum.c,v 1.3 1999/03/08 07:33:54 gernot Exp $ */
/** isalnum(c) returns true if c is alphanumeric */
isalnum (c)
     int             c;
{

    if (isalpha (c))
	return (1);
    if (isdigit (c))
	return (1);
    return (0);
}
