# $Id: Makefile,v 1.4 1998/12/28 12:24:23 gernot Exp $
include ../../../Makefile.conf
INCLUDE=-I../../../include
LIB=$(TOP)/lib

CFLAGS= $(KCFLAGS)
ASFLAGS=$(KCFLAGS)

#INCLUDES=-I. -I.. -I../../../include
#DEFINES=-DSIM #-DTLB_CACHE
#OPT=-g
#CFLAGS= $(INCLUDES) $(DEFINES) -fullwarn $(OPT) -mips3 -64
#ASFLAGS=$(INCLUDES) $(DEFINES) -mips3 $(OPT) -64
#M4=m4

TARGETS=libvm.a
OBJECTS=lookup.o as.o vm.o mem.o mpt.o

.SUFFIXES: .c .o .S


all: libvm.a

.S.o:
	$(AS) $(ASFLAGS) -c -o $*.o $*.S
.c.o:
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

as.o: /usr/include/asm.h /usr/include/sys/asm.h /usr/include/sgidefs.h
as.o: /usr/include/regdef.h /usr/include/sys/regdef.h
as.o: /usr/include/sys/fpregdef.h mpt.h
lookup.o: /usr/include/asm.h /usr/include/sys/asm.h /usr/include/sgidefs.h
lookup.o: /usr/include/regdef.h /usr/include/sys/regdef.h
lookup.o: /usr/include/sys/fpregdef.h mpt.h
vm.o: /usr/include/alloca.h mpt.h
