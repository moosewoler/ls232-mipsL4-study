/* $Id: fileno.c,v 1.3 1999/03/08 07:33:45 gernot Exp $ */
#include <stdio.h>

/*************************************************************
 *  fileno(fp)
 *      convert an fp to an fd
 */
fileno (fp)
     FILE           *fp;
{

    return (fp->fd);
}

/*************************************************************
 *  clearerr(fp)
 *      Clear file read error. Provided as a stub to aid in porting
 *      customer applications to run under PMON.
 */
void
clearerr (fp)
     FILE           *fp;
{
}

/*************************************************************
 *  ferror(fp)
 *      Return file read error status. Provided as a stub to aid in 
 *      porting customer applications to run under PMON.
 */
ferror (fp)
     FILE           *fp;
{
    return (0);
}
