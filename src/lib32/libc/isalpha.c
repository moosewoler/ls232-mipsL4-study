/* $Id: isalpha.c,v 1.3 1999/03/08 07:33:54 gernot Exp $ */
/** isalpha(c) returns true if c is alphabetic */
isalpha (c)
     int             c;
{
    if (c >= 'a' && c <= 'z')
	return (1);
    if (c >= 'A' && c <= 'Z')
	return (1);
    return (0);
}
