/* $Id: realloc.c,v 1.3 1999/03/08 07:34:14 gernot Exp $ */
char           *malloc ();

char           *
realloc (ptr, size)
     char           *ptr;
     unsigned int    size;
{
    char           *p;
    unsigned int    sz;

    p = malloc (size);
    if (!p)
	return (p);
    sz = allocsize (ptr);
    memcpy (p, ptr, (sz > size) ? size : sz);
    free (ptr);
    return (p);
}
