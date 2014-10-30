/* $Id: fflush.c,v 1.3 1999/03/08 07:33:44 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  fflush(fp) flush output buffer
 *      Output in PMON is not buffered, so no flush is necessary
 */
fflush (fp)
     FILE           *fp;
{
}
