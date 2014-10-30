# $Id: Makefile,v 1.6 1998/12/28 12:24:03 gernot Exp $
include ../../../Makefile.conf
INCLUDE=-I../../../include
LIB=$(TOP)/lib

CFLAGS= $(KCFLAGS) -DGPT_NODE_SIZE=$(GPT_NODE_SIZE)
ASFLAGS=$(KCFLAGS) -DGPT_NODE_SIZE=$(GPT_NODE_SIZE)

#INCLUDES=-I.. -I../../../include
#OPT=-DSIM -g

TARGETS=libvm.a
OBJECTS=gpt.o vm.o as.o mem.o lookup.o

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
	makedepend $(INCLUDE) *.s *.c



# DO NOT DELETE

gpt.o: gpt.h /usr/include/alloca.h
mem.o: gpt.h
vm.o: /usr/include/alloca.h gpt.h
