# $Id: Makefile,v 1.9 1999/03/22 06:29:54 gernot Exp $
include ../../Makefile.conf

CFLAGS= $(KCFLAGS)

ASFLAGS=$(KASFLAGS)

TARGETS=libindy.a
OBJECTS=boot.o serial.o serial_init.o


all: $(TARGETS)

libindy.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)

install:


clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
depend:
	makedepend $(INCLUDE) *.S *.c
# DO NOT DELETE

boot.o: ../../../include/regdef.h ../../../include/kernel/p4000i.h
boot.o: ../../../include/r4kc0.h
serial_init.o: ../../../include/asm.h ../../../include/regdef.h
serial_init.o: ../../../include/kernel/indy.h ../../../include/r4kc0.h
