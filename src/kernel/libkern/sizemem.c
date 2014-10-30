#include <r4kc0.h>
#include <kernel/kutils.h>
#include <kernel/kernel.h>

#define MAXMEM	(128ul*1024ul*1024ul)		/* max memory 128 Mb */
#define INCR	(256ul*1024ul)		/* 256 Kb increments */
#define MAXSEGS (MAXMEM / INCR)

#define MARKER	((unsigned char)0x55)
#define NMARKER	((unsigned char)0xaa)

#define START	((unsigned char)0x12)
#define NSTART	((unsigned char)0x34)

#define BW		16	/* 64-bit bus, bank interleaved */

unsigned long sizemem (volatile unsigned char *base, long maxsize)
{
  unsigned char  old[MAXSEGS][2];
  volatile unsigned char  *p;
  unsigned long max, berr;
  long i;
  
  /* round base up to INCR boundary */
  base = (unsigned char *) PHYS_TO_CKSEG1 (((CKSEG0_TO_PHYS ((unsigned long)base) + INCR - 1) & ~(INCR - 1)));
  max = (maxsize - CKSEG1_TO_PHYS ((unsigned long)base)) / INCR;
  if (max > MAXSEGS)
    max = MAXSEGS;
    
  /* load a new bus err vec ****************/
  /* berr = sbdberrenb (0);*/

  /* save contents */
  for (i = 0, p = base; i < max; i++, p += INCR)
  {
    old[i][0] = p[0];
    old[i][1] = p[BW];
  }

  /* set boundaries */
  for (i = 0, p = base; i < max; i++, p += INCR)
  {
    p[0]	= MARKER;
    p[BW]	= NMARKER;
  }

  /* clear first location (in case addresses wrap) */
  base[0] 	= START;
  base[BW]	= NSTART;
  wbflush ();

  /* search for wrap or garbage */
  /*(void) sbdberrenb (0);*/ 	/* reset bus err counter */
  for (i = 1, p = base + INCR;
       i < max && p[0] == MARKER && p[BW] == NMARKER /* && sbdberrcnt () == 0 */;
       i++, p += INCR)
    continue;

  if (i < max && p[0] == START && p[BW] == NSTART)
    /* wraparound: work out distance from base */
    max = p - base;
  else
    /* garbage: dropped off end of memory */
    max = CKSEG1_TO_PHYS ((unsigned long)p);
  
  /* restore contents */
  for (i--, p -= INCR; i >= 0; i--, p -= INCR) {
    p[0]	= old[i][0];
    p[BW]	= old[i][1];
  }
  /* restore old counter */
  /* (void) sbdberrenb (berr);*/
  return (max);
}
