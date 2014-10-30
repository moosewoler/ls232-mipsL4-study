#include <asm.h>
#include <regdef.h>

PROC(wbflush)
	sync
	jr	ra
END(wbflush)
