# $Id: Makefile,v 1.6 1998/12/28 12:24:30 gernot Exp $
include ../../../Makefile.conf
INCLUDE=-I../../../include
LIB=$(TOP)/lib

CFLAGS= $(KCFLAGS)

ASFLAGS=$(KCFLAGS)
#CFLAGS= -O2 -Wall #-DNDEBUG -O2 #-pg
#CFLAGS= -O2 -std1
#INCLUDES=-I.. -I../../../include
#CFLAGS= $(INCLUDES) -DSIM -fullwarn -g -mips3 -64
#ASFLAGS=  $(INCLUDES) -DSIM -mips3 -g -64

TARGETS=libvm.a
OBJECTS=gpt.o gpt_f64lookup.o mem.o tlb2.o vm.o check.o

.SUFFIXES: .c .o .ms .S


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

check.o: gptdefs.h
gpt.o: gptdefs.h /usr/include/alloca.h
mem.o: gptdefs.h
vm.o: gptdefs.h
