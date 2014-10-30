#include "string.h"

static char * _getbase(char *,int *);
static int _atob(unsigned long *, char *, int);



#ifdef PMCC
main (argc, argv)
     int             argc;
     char           *argv[];
{
    unsigned long   n;
    int             len;

    if (argc != 3) {
	printf ("arg count %d\n", argc);
	exit (1);
    }
    if (!atob (&len, argv[2], 10)) {
	printf ("bad len\n");
	exit (1);
    }
    if (gethex (&n, argv[1], len))
	printf ("OK: %08x\n", n);
    else
	printf ("ERROR\n");
}
#endif


static char * _getbase (char *p, int *basep)
{
    if (p[0] == '0') {
	switch (p[1]) {
	case 'x':
	    *basep = 16;
	    break;
	case 't': case 'n':
	    *basep = 10;
	    break;
	case 'o':
	    *basep = 8;
	    break;
	default:
	    *basep = 10;
	    return (p);
	}
	return (p + 2);
    }
    *basep = 10;
    return (p);
}


/*************************************************************
 *  _atob(vp,p,base)
 */
static int _atob (unsigned long *vp, char *p, int base)
{
    unsigned long  value, v1, v2;
    char           *q, tmp[20];
    int             digit;

    if (base == 16 && (q = strchr (p, '.')) != 0) {
	if (q - p > sizeof(tmp) - 1)
	    return (0);
	strncpy (tmp, p, q - p);
	tmp[q - p] = '\0';
	if (!_atob (&v1, tmp, 16))
	    return (0);
	q++;
	if (strchr (q, '.'))
	    return (0);
	if (!_atob (&v2, q, 16))
	    return (0);
	*vp = (v1 << 16) + v2;
	return (1);
    }
    value = *vp = 0;
    for (; *p; p++) {
	value *= base;
	if (*p >= '0' && *p <= '9')
	    digit = *p - '0';
	else if (*p >= 'a' && *p <= 'f')
	    digit = *p - 'a' + 10;
	else if (*p >= 'A' && *p <= 'F')
	    digit = *p - 'A' + 10;
	else
	    return (0);
	if (digit >= base)
	    return (0);
	value += digit;
    }
    *vp = value;
    return (1);
}

/*************************************************************
 *  atob(vp,p,base) 
 *      converts p to binary result in vp, rtn 1 on success
 */
int atob (unsigned int *vp, char *p, int base)
{
#if defined(_MIPS_SZLONG) && _MIPS_SZLONG == 64
    unsigned long  v;
#elif __mips >= 3 && defined(_LONGLONG)  ||  \
    defined(_MIPS_SZLONG) && _MIPS_SZLONG == 64
    unsigned long long v;
#else
    unsigned long  v;
#endif

    if (base == 0)
      p = _getbase (p, &base);
    if (_atob (&v, p, base)) {
	*vp = (unsigned int) v; /* should be 32-bit value */
	return (1);
    }
    return (0);
}



/*************************************************************
 *  llatob(vp,p,base) 
 *      converts p to binary result in vp, rtn 1 on success
 */
int llatob (unsigned long *vp, char *p, int base)
{
  if (base == 0)
      p = _getbase (p, &base);
    return _atob (vp, p, base);
}



/*************************************************************
 *  char *btoa(dst,value,base) 
 *      converts value to ascii, result in dst
 */
char * btoa (char *dst, unsigned int value, int base)
{
    char            buf[34], digit;
    int             i, j, rem, neg;

    if (value == 0) {
	dst[0] = '0';
	dst[1] = 0;
	return (dst);
    }
    neg = 0;
    if (base == -10) {
	base = 10;
	if (value & (1L << 31)) {
	    value = (~value) + 1;
	    neg = 1;
	}
    }
    for (i = 0; value != 0; i++) {
	rem = value % base;
	value /= base;
	if (rem >= 0 && rem <= 9)
	    digit = rem + '0';
	else if (rem >= 10 && rem <= 36)
	    digit = (rem - 10) + 'a';
	else
	    digit = '#';
	buf[i] = digit;
    }
    buf[i] = 0;
    if (neg)
	strcat (buf, "-");

/* reverse the string */
    for (i = 0, j = strlen (buf) - 1; j >= 0; i++, j--)
	dst[i] = buf[j];
    dst[i] = 0;
    return (dst);
}

#if __mips >= 3 
/*************************************************************
 *  char *btoa(dst,value,base) 
 *      converts value to ascii, result in dst
 */
char *llbtoa (char *dst, unsigned long value, int base)
{
    char            buf[66], digit;
    int             i, j, rem, neg;

    if (value == 0) {
	dst[0] = '0';
	dst[1] = 0;
	return (dst);
    }
    neg = 0;
    if (base == -10) {
	base = 10;
#ifdef _LONGLONG
	if (value & (1LL << 63)) {
#else
	 if (value & (1L << 63)) {
#endif
	    value = (~value) + 1;
	    neg = 1;
	}
    }
    for (i = 0; value != 0; i++) {
	rem = (int) (value % base);
	value /= base;
	if (rem >= 0 && rem <= 9)
	    digit = rem + '0';
	else if (rem >= 10 && rem <= 36)
	    digit = (rem - 10) + 'a';
	else
	    digit = '#';
	buf[i] = digit;
    }
    buf[i] = 0;
    if (neg)
	strcat (buf, "-");

/* reverse the string */
    for (i = 0, j = strlen (buf) - 1; j >= 0; i++, j--)
	dst[i] = buf[j];
    dst[i] = 0;
    return (dst);
}
#endif

/*************************************************************
 *  gethex(vp,p,n) 
 *      convert n hex digits from p to binary, result in vp, 
 *      rtn 1 on success
 */
int gethex (unsigned long *vp, char *p, int n)
{
    unsigned long   v;
    int             digit;

    for (v = 0; n > 0; n--) {
	if (*p == 0)
	    return (0);
	v <<= 4;
	if (*p >= '0' && *p <= '9')
	    digit = *p - '0';
	else if (*p >= 'a' && *p <= 'f')
	    digit = *p - 'a' + 10;
	else if (*p >= 'A' && *p <= 'F')
	    digit = *p - 'A' + 10;
	else
	    return (0);
	v |= digit;
	p++;
    }
    *vp = v;
    return (1);
}
