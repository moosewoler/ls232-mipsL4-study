/* $Id: iscntrl.c,v 1.3 1999/03/08 07:33:55 gernot Exp $ */
/** iscntrl(c) returns true if c is delete or control character */
iscntrl (c)
     int             c;
{
    if (c == 0x7f)
	return (1);
    if (c < ' ')
	return (1);
    return (0);
}
