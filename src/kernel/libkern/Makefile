include ../../Makefile.conf

CFLAGS= $(KCFLAGS)

ASFLAGS=$(KCFLAGS)

TARGETS=libkern.a
OBJECTS=atob.o ints.o iscntrl.o isdigit.o islower.o isspace.o sprintf.o str_fmt.o\
	strcat.o\
	strchr.o strcmp.o strcpy.o strichr.o strlen.o strncpy.o strtoupp.o\
	toupper.o vsprintf.o wbflush.o


all:	libkern.a

libkern.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)

install:
#	cp libkern.a $(LIB)


clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
depend:
	makedepend $(INCLUDE) *.S *.c
# DO NOT DELETE

ints.o: /home/alanau/mipsL4/include/asm.h
ints.o: /home/alanau/mipsL4/include/regdef.h
ints.o: /home/alanau/mipsL4/include/r4kc0.h
strcmp.o: /home/alanau/mipsL4/include/regdef.h
wbflush.o: /home/alanau/mipsL4/include/asm.h
wbflush.o: /home/alanau/mipsL4/include/regdef.h
atob.o: string.h types.h
isdigit.o: ctype.h
islower.o: ctype.h
sizemem.o: /home/alanau/mipsL4/include/r4kc0.h
sizemem.o: /home/alanau/mipsL4/include/kernel/kutils.h
sizemem.o: /home/alanau/mipsL4/include/kernel/kernel.h
sizemem.o: /home/alanau/mipsL4/include/kernel/types.h
sizemem.o: /home/alanau/mipsL4/include/l4/types.h
sprintf.o: /home/alanau/mipsL4/include/stdarg.h
sprintf.o: /home/alanau/mipsL4/include/sgiargs.h
sprintf.o: /home/alanau/mipsL4/include/sgidefs.h stdio.h
str_fmt.o: string.h types.h
strcat.o: string.h types.h
strchr.o: string.h types.h
strcpy.o: string.h types.h
strichr.o: string.h types.h
strlen.o: string.h types.h
strncpy.o: string.h types.h
strtoupp.o: string.h types.h ctype.h
toupper.o: string.h types.h ctype.h
vsprintf.o: /home/alanau/mipsL4/include/stdarg.h
vsprintf.o: /home/alanau/mipsL4/include/sgiargs.h
vsprintf.o: /home/alanau/mipsL4/include/sgidefs.h stdio.h string.h types.h
vsprintf.o: ctype.h
