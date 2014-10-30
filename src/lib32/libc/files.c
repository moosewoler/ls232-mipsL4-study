/* $Id: files.c,v 1.3 1999/03/08 07:33:46 gernot Exp $ */
#include <termio.h>

/*************************************************************
 *  _mfile[]
 *      A stub to satisfy the linker when building PMON. This is
 *      necessary because the same read() and write() routines are
 *      used by both PMON and the client, and the client supports
 *      ram-based files.
 */

Ramfile         _mfile[] =
{
    {0}};
