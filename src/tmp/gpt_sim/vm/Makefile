#include $(TOP)/src/Makefile.conf
#INCLUDE=-I$(TOP)/include
#LIB=$(TOP)/lib
#CFLAGS= $(KCFLAGS)
#ASFLAGS=$(KCFLAGS)

INCLUDES=-I. -I.. -I../../../include
DEFINES=-DSIM -DIPT_8 #-DCPT_128 -DTLB_CACHE #-DIPT_128 #-DTLB_CACHE
OPT=-g
CFLAGS= $(INCLUDES) $(DEFINES) -fullwarn $(OPT) -mips3 -64
ASFLAGS=$(INCLUDES) $(DEFINES) -mips3 $(OPT) -64

M4=m4

TARGETS=libvm.a
#IPT  	OBJECTS=lookup.o ipt.o unmap.o vm.o mem.o as.o
#GPT 	OBJECTS=as.o gpt.o lookup.o mem.o vm.o
#MPT OBJECTS=as.o mpt.o lookup.o mem.o vm.o
OBJECTS=as.o ipt.o lookup.o mem.o vm.o unmap.o

.SUFFIXES: .c .o .ms .s


all: libvm.a

.s.o:
	$(AS) $(ASFLAGS) -o $*.o $*.s
.c.o:	
	$(CC) $(CFLAGS) -c $*.c
.ms.o:
	$(M4) $*.ms > m4out.s && $(AS) $(ASFLAGS) -o $*.o m4out.s && rm m4out.s

libvm.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)

clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
dep:
	makedepend $(INCLUDES) *.s *.ms *.c



# DO NOT DELETE

as.o: ../../../include/asm.h ../../../include/regdef.h
as.o: ../../../include/r4kc0.h ../../../include/kernel/kernel.h
as.o: ../../../include/kernel/types.h ../../../include/l4/types.h
as.o: ../../../include/kernel/machine.h ipt.h ../../../include/kernel/vm.h
lookup.o: ../../../include/regdef.h ../../../include/asm.h
lookup.o: ../../../include/kernel/kernel.h ../../../include/kernel/types.h
lookup.o: ../../../include/l4/types.h ../../../include/r4kc0.h
lookup.o: ../../../include/kernel/machine.h ipt.h
lookup.o: ../../../include/kernel/vm.h
ipt.o: ../../../include/r4kc0.h ipt.h ../../../include/kernel/types.h
ipt.o: ../../../include/kernel/kernel.h ../../../include/l4/types.h
ipt.o: ../../../include/kernel/vm.h ../../../include/kernel/panic.h
ipt.o: ../../../include/kernel/kmem.h ../../../include/kernel/types.h
ipt.o: ../../../include/kernel/trace.h ../../../include/l4/sigma0.h
ipt.o: ../../../include/alloca.h
mem.o: ipt.h ../../../include/kernel/types.h ../../../include/kernel/kernel.h
mem.o: ../../../include/l4/types.h ../../../include/kernel/vm.h
mem.o: ../../../include/kernel/kmem.h ../../../include/kernel/types.h
mem.o: ../../../include/kernel/panic.h ../../../include/kernel/trace.h
mem.o: ../../../include/r4kc0.h
unmap.o: ../../../include/r4kc0.h ipt.h ../../../include/kernel/types.h
unmap.o: ../../../include/kernel/kernel.h ../../../include/l4/types.h
unmap.o: ../../../include/kernel/vm.h ../../../include/kernel/panic.h
unmap.o: ../../../include/kernel/kmem.h ../../../include/kernel/types.h
unmap.o: ../../../include/kernel/trace.h ../../../include/l4/sigma0.h
unmap.o: ../../../include/alloca.h
vm.o: ../../../include/kernel/vm.h ../../../include/kernel/kernel.h
vm.o: ../../../include/kernel/types.h ../../../include/l4/types.h
vm.o: ../../../include/r4kc0.h ../../../include/alloca.h
vm.o: ../../../include/l4/sigma0.h ipt.h
