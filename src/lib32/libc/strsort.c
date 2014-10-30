/* $Id: strsort.c,v 1.3 1999/03/08 07:34:40 gernot Exp $ */

/*
 * ** Uses a rather ugly bubble sort. Ugh!
 */
strsort (p)
     char           *p;
{
    int             i, flag;
    char            t;

    if (strlen (p) < 2)
	return;
    for (;;) {
	flag = 0;
	for (i = 0; p[i + 1]; i++) {
	    if (p[i] > p[i + 1]) {
		t = p[i + 1];
		p[i + 1] = p[i];
		p[i] = t;
		flag = 1;
	    }
	}
	if (flag == 0)
	    break;
    }
}
