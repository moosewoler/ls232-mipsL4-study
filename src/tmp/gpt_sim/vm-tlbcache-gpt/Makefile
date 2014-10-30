#CFLAGS= -O2 -Wall #-DNDEBUG -O2 #-pg
#CFLAGS= -O2 -std1
INCLUDES=-I.. -I../../../include
CFLAGS= $(INCLUDES) -DSIM -fullwarn -g -mips3 -64
ASFLAGS=  $(INCLUDES) -DSIM -mips3 -g -64
OBJS= machdep.o main.o panic.o kmem.o sigma0.o 
#DEPENDINC=-I/usr/lib/gcc-lib/i486-linux/2.6.3/include
#CC=gcc
CC=cc
M4=m4
TARGETS=libvm.a
OBJECTS=gpt.o gpt_f64lookup.o mem.o tlb2.o vm.o check.o

.SUFFIXES: .c .o .ms .s


all: libvm.a

.s.o:
	$(AS) $(ASFLAGS) -o $*.o $*.s
.c.o:
	$(CC) $(CFLAGS) -c $*.c
.ms.o:
	$(M4) $*.ms > m4out.s && $(AS) $(ASFLAGS) -o $*.o m4out.s && rm m4out.s

libvm.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)

clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
dep:
	makedepend $(INCLUDE) *.s *.c
# DO NOT DELETE

check.o: gptdefs.h
gpt.o: gptdefs.h /usr/include/alloca.h
mem.o: gptdefs.h
vm.o: gptdefs.h
