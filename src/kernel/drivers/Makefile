# $Id: Makefile,v 1.26 1999/02/10 04:55:27 gernot Exp $
include ../../Makefile.conf
FAKETARGETS=$(MACHTARGETS)
DITFLAG=-a

LDFLAGS=$(USERLDFLAGS)
LIBS=$(USERLIBS)
CRT0=$(LIB)/crtS.o
KERNEL=$(INSTALL)/kernel


CFLAGS= $(USERCFLAGS) -DHOST_BAUD=$(HOST_BAUD) #-DSERIAL_DEBUG

ASFLAGS= $(USERASFLAGS)

TARGETS=indy_serial i8254 serial sonic

ifeq ($(TARGET_MACH),U4600)
SEROBJ=intr_serial.o
endif

ifeq ($(TARGET_MACH),P4000)
SEROBJ=mpsc.o serial.o
SONOBJ=sonic.o
endif



ISEROBJ=l_iserial.o  iserial.o
TOBJ=i8254.o i8254_ass.o
CLKOBJ=clock.o


all: $(FAKETARGETS)

indy_serial_driver: $(ISEROBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l ../kernel` -o indy_serial \
		$(CRT0)  $(ISEROBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) indy_serial ../kernel

serial_driver: $(SEROBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l ../kernel` -o serial $(CRT0) \
		$(SEROBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) serial ../kernel

clock_driver: $(CLKOBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l ../kernel` -o clock $(CRT0) \
		$(CLKOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) clock ../kernel

sonic_driver: $(SONOBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l ../kernel` -o sonic $(CRT0) \
		$(SONOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) sonic ../kernel
i8254_driver: $(TOBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l ../kernel` -o i8254 $(CRT0) \
		$(TOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG)  i8254 ../kernel


install:
	cp -p ../kernel $(KERNEL)/$(KERNELNAME)


clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
depend:
	-makedepend $(INCLUDE) *.c
# DO NOT DELETE

clock.o: /home/alanau/mipsL4/include/string.h
clock.o: /home/alanau/mipsL4/include/types.h
clock.o: /home/alanau/mipsL4/include/libc.h
clock.o: /home/alanau/mipsL4/include/assert.h
clock.o: /home/alanau/mipsL4/include/l4/ipc.h
clock.o: /home/alanau/mipsL4/include/l4/types.h
clock.o: /home/alanau/mipsL4/include/l4/syscalls.h
clock.o: /home/alanau/mipsL4/include/l4/sigma0.h
clock.o: /home/alanau/mipsL4/include/kernel/machine.h
clock.o: /home/alanau/mipsL4/include/kernel/gt64010a.h
i8254.o: /home/alanau/mipsL4/include/kernel/indy.h
i8254.o: /home/alanau/mipsL4/include/libc.h
i8254.o: /home/alanau/mipsL4/include/assert.h
i8254.o: /home/alanau/mipsL4/include/l4/ipc.h
i8254.o: /home/alanau/mipsL4/include/l4/types.h
i8254.o: /home/alanau/mipsL4/include/l4/syscalls.h
i8254.o: /home/alanau/mipsL4/include/l4/sigma0.h
intr_serial.o: /home/alanau/mipsL4/include/string.h
intr_serial.o: /home/alanau/mipsL4/include/types.h
intr_serial.o: /home/alanau/mipsL4/include/libc.h
intr_serial.o: /home/alanau/mipsL4/include/assert.h
intr_serial.o: /home/alanau/mipsL4/include/l4/ipc.h
intr_serial.o: /home/alanau/mipsL4/include/l4/types.h
intr_serial.o: /home/alanau/mipsL4/include/l4/syscalls.h
intr_serial.o: /home/alanau/mipsL4/include/l4/sigma0.h
intr_serial.o: /home/alanau/mipsL4/include/kernel/machine.h z85230.h termio.h
iserial.o: /home/alanau/mipsL4/include/stdarg.h
iserial.o: /home/alanau/mipsL4/include/sgiargs.h
iserial.o: /home/alanau/mipsL4/include/sgidefs.h
iserial.o: /home/alanau/mipsL4/include/libc.h
iserial.o: /home/alanau/mipsL4/include/assert.h
iserial.o: /home/alanau/mipsL4/include/l4/types.h
iserial.o: /home/alanau/mipsL4/include/l4/ipc.h
iserial.o: /home/alanau/mipsL4/include/l4/syscalls.h
iserial.o: /home/alanau/mipsL4/include/l4/sigma0.h
mpsc.o: termio.h mpsc.h
serial.o: termio.h /home/alanau/mipsL4/include/r4kc0.h
serial.o: /home/alanau/mipsL4/include/stdarg.h
serial.o: /home/alanau/mipsL4/include/sgiargs.h
serial.o: /home/alanau/mipsL4/include/sgidefs.h
serial.o: /home/alanau/mipsL4/include/libc.h
serial.o: /home/alanau/mipsL4/include/assert.h
serial.o: /home/alanau/mipsL4/include/l4/types.h
serial.o: /home/alanau/mipsL4/include/l4/ipc.h
serial.o: /home/alanau/mipsL4/include/l4/syscalls.h
serial.o: /home/alanau/mipsL4/include/l4/sigma0.h
sonic.o: sonic.h /home/alanau/mipsL4/include/kernel/p4000i.h
sonic.o: /home/alanau/mipsL4/include/libc.h
sonic.o: /home/alanau/mipsL4/include/assert.h
sonic.o: /home/alanau/mipsL4/include/l4/ipc.h
sonic.o: /home/alanau/mipsL4/include/l4/types.h
sonic.o: /home/alanau/mipsL4/include/l4/syscalls.h
sonic.o: /home/alanau/mipsL4/include/l4/sigma0.h
z85230.o: termio.h z85230.h /home/alanau/mipsL4/include/assert.h
