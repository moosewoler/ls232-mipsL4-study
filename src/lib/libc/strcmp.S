#include <regdef.h>

#ifdef TEST
#define strcmp xstrcmp
#endif

/*
** Loop unrolled four times
*/

	.globl	strcmp
	.ent	strcmp
strcmp:
	.set noreorder

	# main loop
1:	lbu	t0, (a0)
	lbu	t1, (a1)
	beq	t0, zero, 2f	# brif end of string
	 lbu	t2, 1(a0)
	bne	t0, t1, 2f	# brif bytes not equal
	 lbu	t3, 1(a1)
	beq	t2, zero, 3f	# brif end of string
	 lbu	a4, 2(a0)
	bne	t2, t3, 3f	# brif bytes not equal
	 lbu	a5, 2(a1)
	beq	a4, zero, 4f	# brif end of string
	 lbu	a6, 3(a0)
	bne	a4, a5, 4f	# brif bytes not equal
	 lbu	a7, 3(a1)
	beq	a6, zero, 5f	# brif end of string
	 daddu	a0, 4
	beq	a6, a7, 1b	# brif bytes equal
	 daddu	a1, 4
5:	j	ra
	 dsubu	v0, a6, a7

2:	j	ra
	 dsubu	v0, t0, t1
3:	j	ra
	 dsubu	v0, t2, t3
4:	j	ra
	 dsubu	v0, a4, a5

	.set reorder
	.end	strcmp
