# $Id: Makefile,v 1.10 1999/02/10 04:56:55 gernot Exp $
include ../../Makefile.conf
INCLUDE=-I../../include
LIB=$(TOP)/lib



CFLAGS=$(USERCFLAGS)

ASFLAGS=$(USERASFLAGS)


TARGETS=libl4.a
OBJECTS=syscalls.o ipc.o assert.o l4_ipc.o l4_types.o time.o

all: $(LIB)/libl4.a

$(LIB)/libl4.a: libl4.a
	cp libl4.a $(LIB)

libl4.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)


clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
depend:
	makedepend $(INCLUDE) *.S *.c

install:
	cp $(LIB)/libl4.a $(INSTALL)/lib

# DO NOT DELETE

assert.o: ../../include/asm.h ../../include/regdef.h
ipc.o: ../../include/regdef.h ../../include/asm.h ../../include/l4/ipc.h
ipc.o: ../../include/l4/types.h ../../include/l4/syscalls.h macros.h
syscalls.o: ../../include/asm.h ../../include/regdef.h
syscalls.o: ../../include/l4/types.h ../../include/l4/syscalls.h macros.h
l4_ipc.o: ../../include/l4/ipc.h ../../include/l4/types.h
l4_types.o: ../../include/l4/types.h
time.o: ../../include/l4/time.h ../../include/l4/types.h
time.o: ../../include/l4/ipc.h
