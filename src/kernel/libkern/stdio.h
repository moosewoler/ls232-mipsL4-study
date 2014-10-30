#ifndef _STDIO_
#define _STDIO_

typedef struct FILE {
	int fd;
	int valid;
	int ungetcflag;
	int ungetchar;
	} FILE;

#include <stdarg.h>

int	fclose	(FILE *);
FILE	*fopen	(const char *, const char *);
int	fgetc	(FILE *);
char	*fgets	(char *, int , FILE *);
int	fread	(void *, int, int, FILE *);
int	fscanf	(FILE *, const char *, ...);
int	getc	(FILE *);
int	getchar	(void);
char	*gets	(char *);
int	scanf	(const char *, ...);
int	sscanf	(const char *, const char *, ...);
int	ungetc	(int, FILE *);

int	fputc	(int , FILE *);
int	fputs	(const char *, FILE *);
int	fwrite	(const void *, int, int, FILE *);
int	putc	(int, FILE *);
int	putchar	(int);
int	puts	(const char *);

int	fprintf	(FILE *, const char *, ...);
int	sprintf	(char *, const char *, ...);
int	printf	(const char *, ...);

int	vfprintf (FILE *, const char *, va_list);
int	vsprintf (char *, const char *, va_list);
int	vprintf	(const char *, va_list);

void	clearerr (FILE *);
int	feof	(FILE *);
int	ferror	(FILE *);
int	fflush	(FILE *);
int	fseek	(FILE *, long int, int);


extern FILE _iob[];

#define stdin	(&_iob[0])
#define stdout	(&_iob[1])
#define stderr	(&_iob[2])

#ifdef OPEN_MAX
#undef OPEN_MAX
#endif
#define OPEN_MAX 8

#define MAXLN 256

#ifndef NULL
#define NULL 0
#endif

#define EOF  (-1)

typedef int iFunc();
typedef int *Addr;

extern void alphn_string(char *);
extern void alphn_set(char *);
#endif /* _STDIO_ */
