# $Id: Makefile,v 1.4 1998/12/28 12:23:53 gernot Exp $
include ../../../Makefile.conf
INCLUDE=-I../../../include
LIB=$(TOP)/lib

CFLAGS= $(KCFLAGS)

ASFLAGS=$(KCFLAGS)

#INCLUDES=-I.. -I../../../include
#OPT=-DSIM -g

TARGETS=libvm.a
OBJECTS=lookup.o ipt.o unmap.o vm.o mem.o as.o

.SUFFIXES: .c .o .S


all: libvm.a

.S.o:	$*.o
	$(AS) $(ASFLAGS) -c -o $*.o $*.S
.c.o:   $*.o
	$(CC) $(CFLAGS) -c $*.c


libvm.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)

clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
depend:
	makedepend $(INCLUDE) *.S *.c



# DO NOT DELETE

ipt.o: /home/kevine/Work/mipsL4/include/r4kc0.h ipt.h
ipt.o: /home/kevine/Work/mipsL4/include/kernel/types.h
ipt.o: /home/kevine/Work/mipsL4/include/kernel/kernel.h
ipt.o: /home/kevine/Work/mipsL4/include/l4/types.h
ipt.o: /home/kevine/Work/mipsL4/include/kernel/vm.h
ipt.o: /home/kevine/Work/mipsL4/include/kernel/panic.h
ipt.o: /home/kevine/Work/mipsL4/include/kernel/kmem.h
ipt.o: /home/kevine/Work/mipsL4/include/kernel/types.h
ipt.o: /home/kevine/Work/mipsL4/include/kernel/trace.h
ipt.o: /home/kevine/Work/mipsL4/include/l4/sigma0.h
ipt.o: /home/kevine/Work/mipsL4/include/alloca.h
mem.o: ipt.h /home/kevine/Work/mipsL4/include/kernel/types.h
mem.o: /home/kevine/Work/mipsL4/include/kernel/kernel.h
mem.o: /home/kevine/Work/mipsL4/include/l4/types.h
mem.o: /home/kevine/Work/mipsL4/include/kernel/vm.h
mem.o: /home/kevine/Work/mipsL4/include/kernel/kmem.h
mem.o: /home/kevine/Work/mipsL4/include/kernel/types.h
mem.o: /home/kevine/Work/mipsL4/include/kernel/panic.h
mem.o: /home/kevine/Work/mipsL4/include/kernel/trace.h
mem.o: /home/kevine/Work/mipsL4/include/r4kc0.h
unmap.o: /home/kevine/Work/mipsL4/include/r4kc0.h ipt.h
unmap.o: /home/kevine/Work/mipsL4/include/kernel/types.h
unmap.o: /home/kevine/Work/mipsL4/include/kernel/kernel.h
unmap.o: /home/kevine/Work/mipsL4/include/l4/types.h
unmap.o: /home/kevine/Work/mipsL4/include/kernel/vm.h
unmap.o: /home/kevine/Work/mipsL4/include/kernel/panic.h
unmap.o: /home/kevine/Work/mipsL4/include/kernel/kmem.h
unmap.o: /home/kevine/Work/mipsL4/include/kernel/types.h
unmap.o: /home/kevine/Work/mipsL4/include/kernel/trace.h
unmap.o: /home/kevine/Work/mipsL4/include/l4/sigma0.h
unmap.o: /home/kevine/Work/mipsL4/include/alloca.h
vm.o: /home/kevine/Work/mipsL4/include/kernel/vm.h
vm.o: /home/kevine/Work/mipsL4/include/kernel/kernel.h
vm.o: /home/kevine/Work/mipsL4/include/kernel/types.h
vm.o: /home/kevine/Work/mipsL4/include/l4/types.h
vm.o: /home/kevine/Work/mipsL4/include/r4kc0.h
vm.o: /home/kevine/Work/mipsL4/include/alloca.h
vm.o: /home/kevine/Work/mipsL4/include/l4/sigma0.h ipt.h
