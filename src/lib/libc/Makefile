include ../../Makefile.conf
INCLUDE=-I../../include
LIB=$(TOP)/lib

CFLAGS=$(USERCFLAGS)

ASFLAGS=$(USERASFLAGS)

TARGETS=libc.a
OBJECTS=atob.o iscntrl.o isdigit.o islower.o isspace.o sprintf.o str_fmt.o\
	strcat.o\
	strchr.o strcmp.o strcpy.o strichr.o strlen.o strncpy.o strtoupp.o\
	toupper.o vsprintf.o wbflush.o


all:	$(LIB)/libc.a


ifeq ($(TOOL_CHAIN),SGI)
sprintf.o:	sprintf.c
	/usr/lib/cpp $(MACH) -nostdinc $(INCLUDE) sprintf.c > sprintf.i
	$(CC) -O2 -G 0 -mips3 $(ABI_OPT) $(C_ABI_OPT) $(C_OTH_OPT) -c sprintf.c
endif

$(LIB)/libc.a: libc.a
	cp libc.a $(LIB)


libc.a: $(OBJECTS)
	$(AR) crs $@ $(OBJECTS)


clean:
	rm *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
depend:
	makedepend $(INCLUDE) *.S *.c

install:	all
	cp $(LIB)/libc.a $(INSTALL)/lib
# DO NOT DELETE

strcmp.o: ../../include/regdef.h
wbflush.o: ../../include/asm.h ../../include/regdef.h
atob.o: ../../include/string.h ../../include/types.h
isdigit.o: ctype.h
islower.o: ctype.h
sprintf.o: ../../include/stdarg.h ../../include/sgiargs.h
sprintf.o: ../../include/sgidefs.h ../../include/stdio.h
sprintf.o: ../../include/varargs.h
str_fmt.o: ../../include/string.h ../../include/types.h
strcat.o: ../../include/string.h ../../include/types.h
strchr.o: ../../include/string.h ../../include/types.h
strcpy.o: ../../include/string.h ../../include/types.h
strichr.o: ../../include/string.h ../../include/types.h
strlen.o: ../../include/string.h ../../include/types.h
strncpy.o: ../../include/string.h ../../include/types.h
strtoupp.o: ../../include/string.h ../../include/types.h ctype.h
toupper.o: ../../include/string.h ../../include/types.h ctype.h
vsprintf.o: ../../include/stdarg.h ../../include/sgiargs.h
vsprintf.o: ../../include/sgidefs.h ../../include/stdio.h
vsprintf.o: ../../include/varargs.h ../../include/string.h
vsprintf.o: ../../include/types.h ctype.h
