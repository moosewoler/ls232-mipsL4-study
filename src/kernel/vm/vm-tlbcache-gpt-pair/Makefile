# $Id: Makefile,v 1.7 1999/02/10 04:56:31 gernot Exp $
include ../../../Makefile.conf
INCLUDE=-I../../../include
LIB=$(TOP)/lib

CFLAGS= $(KCFLAGS)

ASFLAGS=$(KCFLAGS)

#INCLUDES=-I.. -I../../../include
#OPT=-DSIM -g

TARGETS=libvm.a
OBJECTS=gpt.o vm.o as.o mem.o lookup.o $(TLB2_OBJ)


all: libvm.a


libvm.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)

clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
depend:
	makedepend $(INCLUDE) *.S *.c



# DO NOT DELETE

as.o: ../../../include/asm.h ../../../include/regdef.h
as.o: ../../../include/r4kc0.h ../../../include/kernel/kernel.h
as.o: ../../../include/kernel/types.h ../../../include/l4/types.h
as.o: ../../../include/kernel/machine.h gpt.h ../../../include/kernel/vm.h
lookup.o: ../../../include/regdef.h ../../../include/asm.h
lookup.o: ../../../include/kernel/kernel.h ../../../include/kernel/types.h
lookup.o: ../../../include/l4/types.h ../../../include/r4kc0.h gpt.h
lookup.o: ../../../include/kernel/vm.h
tlb2-1way-128.o: ../../../include/asm.h ../../../include/regdef.h
tlb2-1way-128.o: ../../../include/r4kc0.h ../../../include/kernel/kernel.h
tlb2-1way-128.o: ../../../include/kernel/types.h ../../../include/l4/types.h
tlb2-1way-128.o: ../../../include/kernel/machine.h gpt.h
tlb2-1way-128.o: ../../../include/kernel/vm.h
tlb2-1way-8.o: ../../../include/asm.h ../../../include/regdef.h
tlb2-1way-8.o: ../../../include/r4kc0.h ../../../include/kernel/kernel.h
tlb2-1way-8.o: ../../../include/kernel/types.h ../../../include/l4/types.h
tlb2-1way-8.o: ../../../include/kernel/machine.h gpt.h
tlb2-1way-8.o: ../../../include/kernel/vm.h
tlb2-2way-128.o: ../../../include/asm.h ../../../include/regdef.h
tlb2-2way-128.o: ../../../include/r4kc0.h ../../../include/kernel/kernel.h
tlb2-2way-128.o: ../../../include/kernel/types.h ../../../include/l4/types.h
tlb2-2way-128.o: ../../../include/kernel/machine.h gpt.h
tlb2-2way-128.o: ../../../include/kernel/vm.h
tlb2-2way-8.o: ../../../include/asm.h ../../../include/regdef.h
tlb2-2way-8.o: ../../../include/r4kc0.h ../../../include/kernel/kernel.h
tlb2-2way-8.o: ../../../include/kernel/types.h ../../../include/l4/types.h
tlb2-2way-8.o: ../../../include/kernel/machine.h gpt.h
tlb2-2way-8.o: ../../../include/kernel/vm.h
gpt.o: ../../../include/r4kc0.h gpt.h ../../../include/kernel/types.h
gpt.o: ../../../include/kernel/kernel.h ../../../include/l4/types.h
gpt.o: ../../../include/kernel/vm.h ../../../include/kernel/panic.h
gpt.o: ../../../include/kernel/kmem.h ../../../include/kernel/types.h
gpt.o: ../../../include/kernel/trace.h ../../../include/l4/sigma0.h
gpt.o: ../../../include/alloca.h
mem.o: gpt.h ../../../include/kernel/types.h ../../../include/kernel/kernel.h
mem.o: ../../../include/l4/types.h ../../../include/kernel/vm.h
mem.o: ../../../include/kernel/kmem.h ../../../include/kernel/types.h
mem.o: ../../../include/kernel/panic.h ../../../include/kernel/trace.h
mem.o: ../../../include/r4kc0.h
vm.o: ../../../include/kernel/vm.h ../../../include/kernel/kernel.h
vm.o: ../../../include/kernel/types.h ../../../include/l4/types.h
vm.o: ../../../include/r4kc0.h ../../../include/alloca.h
vm.o: ../../../include/kernel/types.h ../../../include/l4/sigma0.h gpt.h
