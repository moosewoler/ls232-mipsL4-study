/* $Id: bcopy.c,v 1.3 1999/03/08 07:33:34 gernot Exp $ */
/** bcopy(src,dst,bytes) copy bytes from src to destination */
bcopy (src, dst, bytes)
     char           *src, *dst;
     unsigned int   bytes;
{

    if (dst >= src && dst < src + bytes) {
	/* do overlapping copy backwards, slowly! */
	src += bytes;
	dst += bytes;
	while (bytes--)
	  *--dst = *--src;
    } else {
	/* use the assembler code memcpy() */
	memcpy (dst, src, bytes);
    }
}
