#ifndef _STRING_
#define _STRING_

#include <types.h>

char *strncat(),cc2str(),*rindex();
char *strdchr(),*strposn(),*getword(),*index();
char *strset(),*strrset(),*strrchr(),*strbalp(),*strrpset(),*strpbrk();
char *strtok();
int strequ(),strlequ(),strpat();

/* definitions for fmt parameter of str_fmt(p,width,fmt) */
#define FMT_RJUST 0
#define FMT_LJUST 1
#define FMT_RJUST0 2
#define FMT_CENTER 3

extern char *strcat(char *, char *);
extern char *strncpy(char *, const char *, size_t);
extern char *strchr(char *, char);
extern char *strcpy(char *, const char *);
extern char *strichr (char *p, int c);
extern size_t strlen (char *);
extern int strcmp (const char *, const char*);


extern int atob (unsigned int *vp, char *p, int base);
extern char *btoa(char *, unsigned int, int);
extern char *llbtoa(char *, unsigned long, int);
int gethex (unsigned long *vp, char *p, int n);

extern void strtoupper(char *);
extern void str_fmt(char *, int, int);
#endif /* _STRING_ */
