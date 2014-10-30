#CFLAGS= -O2 -Wall #-DNDEBUG -O2 #-pg
#CFLAGS= -O2 -std1
TOP=/import/liszt/1/alanau/l4/mipsL4
INCLUDES=-I. -I../../include
DEFINES=-DSIM #-DTLB_CACHE
OPT=-g
CFLAGS= $(INCLUDES) $(DEFINES) -fullwarn $(OPT) -mips3 -64
ASFLAGS=$(INCLUDES) $(DEFINES) -mips3 $(OPT) -64
OBJS= machdep.o main.o panic.o kmem.o sigma0.o
#DEPENDINC=-I/usr/lib/gcc-lib/i486-linux/2.6.3/include
#CC=gcc
CC=cc
M4=m4

SUBDIRS=vm

.SUFFIXES: .c .o .h .ms .s
TARGETS=main 
.c.o:
	$(CC) $(CFLAGS) -c $*.c
.s.o:
	$(AS) $(ASFLAGS) -o $*.o $*.s

.ms.o:
	$(M4) $*.ms > m4out.s && $(AS) $(ASFLAGS) -o $*.o m4out.s && rm m4out.s


all: subdirs $(TARGETS)

main: $(OBJS) vm/libvm.a 
	$(CC) $(CFLAGS) -o main $(OBJS) vm/libvm.a
subdirs:
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' all) ;\
	done
dep:
	makedepend $(INCLUDES) *.c
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' dep) ;\
	done
clean:
	rm -f $(TARGETS) *.o 
realclean:
	rm -f $(TARGETS) *.o *.log *~ gmon.out *.bak

# DO NOT DELETE THIS LINE -- make depend depends on it.

kmem.o: ../../include/stdio.h ../../include/varargs.h /usr/include/stdlib.h
kmem.o: /usr/include/standards.h ../../include/sgidefs.h
kmem.o: ../../include/kernel/kmem.h types.h ../../include/kernel/panic.h
kmem.o: ../../include/r4kc0.h sim.h ../../include/l4/types.h
kmem.o: ../../include/kernel/types.h ../../include/kernel/kernel.h
machdep.o: ../../include/kernel/vm.h panic.h /usr/include/stdlib.h
machdep.o: /usr/include/standards.h ../../include/sgidefs.h
machdep.o: /usr/include/unistd.h /usr/include/math.h ../../include/stdio.h
machdep.o: ../../include/varargs.h
main.o: ../../include/kernel/vm.h ../../include/kernel/kmem.h types.h
main.o: ../../include/r4kc0.h ../../include/stdio.h ../../include/varargs.h
main.o: /usr/include/stdlib.h /usr/include/standards.h
main.o: ../../include/sgidefs.h ../../include/assert.h debug.h sim.h
main.o: ../../include/l4/types.h ../../include/kernel/types.h
main.o: ../../include/kernel/kernel.h
panic.o: ../../include/stdio.h ../../include/varargs.h /usr/include/stdlib.h
panic.o: /usr/include/standards.h ../../include/sgidefs.h panic.h
sigma0.o: ../../include/kernel/vm.h panic.h kmem.h /usr/include/stdlib.h
sigma0.o: /usr/include/standards.h ../../include/sgidefs.h
sigma0.o: ../../include/stdio.h ../../include/varargs.h
