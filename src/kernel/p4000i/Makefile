# $Id: Makefile,v 1.13 1999/03/22 06:26:29 gernot Exp $
include ../../Makefile.conf

CFLAGS= $(KCFLAGS) -DHOST_BAUD=$(HOST_BAUD)

ASFLAGS=$(KASFLAGS)

TARGETS=libp4000i.a
OBJECTS=alphn.o boot.o mpsc.o serial.o serial_init.o


all: $(TARGETS)

libp4000i.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)

install:


clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
depend:
	makedepend $(INCLUDE) *.S *.c
# DO NOT DELETE

alphn.o: ../../../include/regdef.h ../../../include/kernel/p4000i.h
alphn.o: ../../../include/r4kc0.h ../../../include/asm.h
boot.o: ../../../include/regdef.h ../../../include/kernel/p4000i.h
boot.o: ../../../include/r4kc0.h
serial_init.o: ../../../include/asm.h ../../../include/regdef.h
serial_init.o: ../../../include/kernel/p4000i.h ../../../include/r4kc0.h
mpsc.o: termio.h mpsc.h
serial.o: mpsc.h termio.h ../../../include/r4kc0.h ../../../include/stdarg.h
serial.o: ../../../include/kernel/panic.h ../../../include/kernel/kutils.h
serial.o: ../../../include/kernel/trace.h ../../../include/l4/types.h
serial.o: ../../../include/l4/ipc.h ../../../include/l4/sigma0.h
