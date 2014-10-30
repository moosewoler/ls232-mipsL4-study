# $Id: Makefile,v 1.13 1999/02/10 04:56:19 gernot Exp $
include ../../Makefile.conf

CFLAGS= $(KCFLAGS) -DHOST_BAUD=$(HOST_BAUD)

ASFLAGS=$(KCFLAGS)

TARGETS=libu4600.a
OBJECTS=boot.o z85230.o serial.o dlr1414.o


all: $(TARGETS)

libu4600.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)

install:


clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
depend:
	makedepend $(INCLUDE) *.S *.c
# DO NOT DELETE

boot.o: /home/alanau/mipsL4/include/regdef.h
boot.o: /home/alanau/mipsL4/include/kernel/machine.h
boot.o: /home/alanau/mipsL4/include/r4kc0.h gt64010a.h
dlr1414.o: /home/alanau/mipsL4/include/regdef.h
dlr1414.o: /home/alanau/mipsL4/include/asm.h
dlr1414.o: /home/alanau/mipsL4/include/kernel/machine.h
dlr1414.o: /home/alanau/mipsL4/include/r4kc0.h
dlr1414.o: /home/alanau/mipsL4/include/kernel/u4600.h
serial.o: z85230.h termio.h /home/alanau/mipsL4/include/r4kc0.h
serial.o: /home/alanau/mipsL4/include/stdarg.h
serial.o: /home/alanau/mipsL4/include/sgiargs.h
serial.o: /home/alanau/mipsL4/include/sgidefs.h
serial.o: /home/alanau/mipsL4/include/kernel/panic.h
serial.o: /home/alanau/mipsL4/include/kernel/kutils.h
serial.o: /home/alanau/mipsL4/include/kernel/trace.h
serial.o: /home/alanau/mipsL4/include/kernel/u4600.h
serial.o: /home/alanau/mipsL4/include/l4/types.h
serial.o: /home/alanau/mipsL4/include/l4/ipc.h
serial.o: /home/alanau/mipsL4/include/l4/sigma0.h
z85230.o: termio.h z85230.h
