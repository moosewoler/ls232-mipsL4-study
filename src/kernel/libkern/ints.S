#include <asm.h>
#include <regdef.h>
#include <r4kc0.h>

PROC(ints_on)
	.set	noreorder
	mfc0	t0, C0_STATUS
	ori	t0, t0, ST_IE
	mtc0	t0, C0_STATUS
	jr	ra
	nop
	.set	reorder
END(ints_on)

PROC(ints_off)
	.set	noreorder
	mfc0	t0, C0_STATUS
	li	t1, ~ST_IE
	and	t0, t0, t1
	mtc0	t0, C0_STATUS
	jr	ra
	nop
	.set	reorder
END(ints_off)

PROC(get_count)
	.set	noreorder
	mfc0	t0, C0_COUNT
	sw	t0, (a0)
	mtc0	zero, C0_COUNT
	jr	ra
	nop
	.set	reorder
END(get_count)

PROC(zero_count)
	.set noreorder
	mtc0	zero, C0_COUNT
	jr	ra
	nop
	.set reorder
END(zero_count)
