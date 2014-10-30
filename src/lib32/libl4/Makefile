# $Id: Makefile,v 1.3 1999/03/08 07:35:21 gernot Exp $
include ../../Makefile.conf
INCLUDE=-I../../include
LIB=$(TOP)/lib


CC=$(CC32)
CFLAGS=$(CFLAGS32)
ABI_OPT= $(ABI_OPT32)
C_ABI_OPT= $(C_ABI_OPT32)
LD_ABI_OPT= $(LD_ABI_OPT32)

ASFLAGS=$(ASFLAGS32)
AS=$(AS32)

AR=$(AR32)

TARGETS=libl4.a
CFILES=l4.c l4_types.c l4_ipc.c
AFILES=ipc.S assert.S syscalls.S
OBJECTS=$(CFILES:.c=.o) $(AFILES:.S=.o) 

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
	cp $(LIB)/libl4.a $(INSTALL)/lib32

# DO NOT DELETE

l4.o: /home/alanau/work/mipsL4/include/l4/ipc.h
l4.o: /home/alanau/work/mipsL4/include/l4/types.h
l4.o: /home/alanau/work/mipsL4/include/l4/syscalls.h
l4_ipc.o: /home/alanau/work/mipsL4/include/l4/ipc.h
l4_ipc.o: /home/alanau/work/mipsL4/include/l4/types.h
l4_types.o: /home/alanau/work/mipsL4/include/l4/types.h
