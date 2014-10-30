/* $Id: isspace.c,v 1.3 1999/03/08 07:34:01 gernot Exp $ */
/** isspace(c) returns 1 if c == tab newline or space */
isspace (c)
     int             c;
{
    switch (c) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
    case '\f':
    case '\v':
	return (1);
    }
    return (0);
}
