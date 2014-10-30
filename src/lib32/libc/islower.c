/* $Id: islower.c,v 1.3 1999/03/08 07:33:59 gernot Exp $ */
/** islower(c) returns true if c is lower case */
islower (c)
     int             c;
{
    if (c >= 'a' && c <= 'z')
	return (1);
    return (0);
}


/** islower(c) returns true if c is lower case */
isupper (c)
     int             c;
{
    if (c >= 'A' && c <= 'Z')
	return (1);
    return (0);
}
