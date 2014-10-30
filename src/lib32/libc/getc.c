/* $Id: getc.c,v 1.3 1999/03/08 07:33:49 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  getc(fp) get char from stream
 */
getc (fp)
     FILE           *fp;
{
    return (fgetc (fp));
}
