include $(TOP)/src/Makefile.conf
INCLUDE=-I../../include
LIB=$(TOP)/lib32

CC=$(CC32)
CFLAGS=$(CFLAGS32)
ABI_OPT= $(ABI_OPT32)
C_ABI_OPT= $(C_ABI_OPT32)
LD_ABI_OPT= $(LD_ABI_OPT32)

ASFLAGS=$(ASFLAGS32)
AS=$(AS32)

AR=$(AR32)

TARGETS=libc.a

# not used
# termio.c files.c signal.c sizemem.c lseek.c  stdio.c terms.c
# calloc.c open.c close.c write.c queue.c  feof.c read.c
#  crt1.S haswhat.S modf.S  sable.S longjmp.S setbs.S cp0supp.S
# cp2supp.S fabs.S r3kcflu.S fpstate.S crtn.S  r33kcflu.S r4kcflu.S

CFILES=argvize.c   fgetc.c     iscntrl.c   rindex.c    strequ.c    strsort.c\
atob.c      fgets.c     isdigit.c   sbrk.c      strichr.c   strspn.c\
atod.c      fileno.c    islower.c   scanf.c     striequ.c   strstr.c\
atof.c      isprint.c   stristr.c   strtok.c\
atoi.c      fprintf.c   isspace.c   strlen.c    strtoupp.c\
atol.c      fputs.c     isxdigit.c  sprintf.c   strmerge.c\
bcopy.c     fread.c     strncat.c  \
bzero.c     fseek.c     malloc.c    str2cc.c    strncmp.c   time.c\
fwrite.c    str_fmt.c   strncpy.c   toupper.c\
cc2str.c    getc.c      printf.c    strbalp.c   strnwrd.c   ungetc.c\
getchar.c   putc.c      strbequ.c   strpat.c    vfprintf.c\
dbl2asci.c  getmach.c   putchar.c   strcat.c    strpbrk.c   vsprintf.c\
eraline.c   getopt.c    puts.c      strccat.c   strposn.c   \
errno.c     gets.c      qsort.c     strchr.c    strrchr.c\
exit.c      getword.c   strcpy.c    strrpset.c\
fclose.c    index.c     rand.c      strcspn.c   strrrot.c\
isalnum.c   strdchr.c   strrset.c\
fflush.c    isalpha.c   realloc.c   strempty.c  strset.c

AFILES=abort.S clock.S memcpy.S strcmp.S memset.S    

OBJECTS=$(CFILES:.c=.o) $(AFILES:.S=.o) 



all: $(LIB)/libc.a

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
	cp $(LIB)/libc.a $(INSTALL)/lib32
# DO NOT DELETE

atob.o: /home/alanau/work/mipsL4/include/string.h
atoi.o: /home/alanau/work/mipsL4/include/string.h
atol.o: /home/alanau/work/mipsL4/include/string.h
calloc.o: /usr/include/malloc.h
cc2str.o: /home/alanau/work/mipsL4/include/stdio.h
cc2str.o: /home/alanau/work/mipsL4/include/varargs.h
cc2str.o: /home/alanau/work/mipsL4/include/string.h
close.o: /usr/include/termio.h /usr/include/sys/termio.h
close.o: /usr/include/sys/termios.h /usr/include/sys/feature_tests.h
close.o: /usr/include/sys/ttydev.h /usr/include/sys/types.h
close.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
close.o: /usr/include/sys/select.h /usr/include/sys/time.h
close.o: /usr/include/time.h
fclose.o: /home/alanau/work/mipsL4/include/stdio.h
fclose.o: /home/alanau/work/mipsL4/include/varargs.h
feof.o: /usr/include/termio.h /usr/include/sys/termio.h
feof.o: /usr/include/sys/termios.h /usr/include/sys/feature_tests.h
feof.o: /usr/include/sys/ttydev.h /usr/include/sys/types.h
feof.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
feof.o: /usr/include/sys/select.h /usr/include/sys/time.h /usr/include/time.h
fflush.o: /home/alanau/work/mipsL4/include/stdio.h
fflush.o: /home/alanau/work/mipsL4/include/varargs.h
fgetc.o: /home/alanau/work/mipsL4/include/stdio.h
fgetc.o: /home/alanau/work/mipsL4/include/varargs.h
fgets.o: /home/alanau/work/mipsL4/include/stdio.h
fgets.o: /home/alanau/work/mipsL4/include/varargs.h
fileno.o: /home/alanau/work/mipsL4/include/stdio.h
fileno.o: /home/alanau/work/mipsL4/include/varargs.h
files.o: /usr/include/termio.h /usr/include/sys/termio.h
files.o: /usr/include/sys/termios.h /usr/include/sys/feature_tests.h
files.o: /usr/include/sys/ttydev.h /usr/include/sys/types.h
files.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
files.o: /usr/include/sys/select.h /usr/include/sys/time.h
files.o: /usr/include/time.h
fprintf.o: /home/alanau/work/mipsL4/include/stdarg.h
fprintf.o: /home/alanau/work/mipsL4/include/stdio.h
fprintf.o: /home/alanau/work/mipsL4/include/varargs.h
fputs.o: /home/alanau/work/mipsL4/include/stdio.h
fputs.o: /home/alanau/work/mipsL4/include/varargs.h
fread.o: /home/alanau/work/mipsL4/include/stdio.h
fread.o: /home/alanau/work/mipsL4/include/varargs.h
fseek.o: /home/alanau/work/mipsL4/include/stdio.h
fseek.o: /home/alanau/work/mipsL4/include/varargs.h
fwrite.o: /home/alanau/work/mipsL4/include/stdio.h
fwrite.o: /home/alanau/work/mipsL4/include/varargs.h
getc.o: /home/alanau/work/mipsL4/include/stdio.h
getc.o: /home/alanau/work/mipsL4/include/varargs.h
getchar.o: /home/alanau/work/mipsL4/include/stdio.h
getchar.o: /home/alanau/work/mipsL4/include/varargs.h
getopt.o: /home/alanau/work/mipsL4/include/stdio.h
getopt.o: /home/alanau/work/mipsL4/include/varargs.h
getopt.o: /home/alanau/work/mipsL4/include/string.h
gets.o: /home/alanau/work/mipsL4/include/stdio.h
gets.o: /home/alanau/work/mipsL4/include/varargs.h
getword.o: /home/alanau/work/mipsL4/include/string.h
index.o: /home/alanau/work/mipsL4/include/string.h
lseek.o: /usr/include/termio.h /usr/include/sys/termio.h
lseek.o: /usr/include/sys/termios.h /usr/include/sys/feature_tests.h
lseek.o: /usr/include/sys/ttydev.h /usr/include/sys/types.h
lseek.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
lseek.o: /usr/include/sys/select.h /usr/include/sys/time.h
lseek.o: /usr/include/time.h /usr/include/errno.h /usr/include/sys/errno.h
lseek.o: /usr/include/unistd.h /usr/include/sys/unistd.h
open.o: /usr/include/termio.h /usr/include/sys/termio.h
open.o: /usr/include/sys/termios.h /usr/include/sys/feature_tests.h
open.o: /usr/include/sys/ttydev.h /usr/include/sys/types.h
open.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
open.o: /usr/include/sys/select.h /usr/include/sys/time.h /usr/include/time.h
printf.o: /home/alanau/work/mipsL4/include/stdarg.h
printf.o: /home/alanau/work/mipsL4/include/stdio.h
printf.o: /home/alanau/work/mipsL4/include/varargs.h
putc.o: /home/alanau/work/mipsL4/include/stdio.h
putc.o: /home/alanau/work/mipsL4/include/varargs.h
putchar.o: /home/alanau/work/mipsL4/include/stdio.h
putchar.o: /home/alanau/work/mipsL4/include/varargs.h
puts.o: /home/alanau/work/mipsL4/include/stdio.h
puts.o: /home/alanau/work/mipsL4/include/varargs.h
read.o: /usr/include/termio.h /usr/include/sys/termio.h
read.o: /usr/include/sys/termios.h /usr/include/sys/feature_tests.h
read.o: /usr/include/sys/ttydev.h /usr/include/sys/types.h
read.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
read.o: /usr/include/sys/select.h /usr/include/sys/time.h /usr/include/time.h
rindex.o: /home/alanau/work/mipsL4/include/string.h
scanf.o: /home/alanau/work/mipsL4/include/stdarg.h
scanf.o: /home/alanau/work/mipsL4/include/stdio.h
scanf.o: /home/alanau/work/mipsL4/include/varargs.h
scanf.o: /home/alanau/work/mipsL4/include/string.h
signal.o: /usr/include/signal.h /usr/include/sys/feature_tests.h
signal.o: /usr/include/sys/signal.h /usr/include/sys/unistd.h
signal.o: /usr/include/termio.h /usr/include/sys/termio.h
signal.o: /usr/include/sys/termios.h /usr/include/sys/ttydev.h
signal.o: /usr/include/sys/types.h /usr/include/sys/isa_defs.h
signal.o: /usr/include/sys/machtypes.h /usr/include/sys/select.h
signal.o: /usr/include/sys/time.h /usr/include/time.h /usr/include/errno.h
signal.o: /usr/include/sys/errno.h
sprintf.o: /home/alanau/work/mipsL4/include/stdarg.h
stdio.o: /home/alanau/work/mipsL4/include/stdarg.h
stdio.o: /home/alanau/work/mipsL4/include/stdio.h
stdio.o: /home/alanau/work/mipsL4/include/varargs.h /usr/include/fcntl.h
stdio.o: /usr/include/sys/types.h /usr/include/sys/feature_tests.h
stdio.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
stdio.o: /usr/include/sys/select.h /usr/include/sys/time.h
stdio.o: /usr/include/time.h /usr/include/sys/fcntl.h
str2cc.o: /home/alanau/work/mipsL4/include/string.h
str_fmt.o: /home/alanau/work/mipsL4/include/string.h
strbalp.o: /home/alanau/work/mipsL4/include/string.h
strbequ.o: /home/alanau/work/mipsL4/include/string.h
strcat.o: /home/alanau/work/mipsL4/include/string.h
strccat.o: /home/alanau/work/mipsL4/include/string.h
strchr.o: /home/alanau/work/mipsL4/include/string.h
strcpy.o: /home/alanau/work/mipsL4/include/string.h
strdchr.o: /home/alanau/work/mipsL4/include/string.h
strempty.o: /home/alanau/work/mipsL4/include/string.h
strequ.o: /home/alanau/work/mipsL4/include/string.h
strichr.o: /home/alanau/work/mipsL4/include/string.h
striequ.o: /home/alanau/work/mipsL4/include/string.h
stristr.o: /home/alanau/work/mipsL4/include/string.h
strlen.o: /home/alanau/work/mipsL4/include/string.h
strncat.o: /home/alanau/work/mipsL4/include/string.h
strncmp.o: /home/alanau/work/mipsL4/include/string.h
strncpy.o: /home/alanau/work/mipsL4/include/string.h
strnwrd.o: /home/alanau/work/mipsL4/include/string.h
strpat.o: /home/alanau/work/mipsL4/include/stdio.h
strpat.o: /home/alanau/work/mipsL4/include/varargs.h
strpat.o: /home/alanau/work/mipsL4/include/string.h
strpbrk.o: /home/alanau/work/mipsL4/include/string.h
strposn.o: /home/alanau/work/mipsL4/include/string.h
strrchr.o: /home/alanau/work/mipsL4/include/string.h
strrpset.o: /home/alanau/work/mipsL4/include/string.h
strrset.o: /home/alanau/work/mipsL4/include/string.h
strset.o: /home/alanau/work/mipsL4/include/string.h
strstr.o: /home/alanau/work/mipsL4/include/string.h
strtok.o: /home/alanau/work/mipsL4/include/string.h
strtoupp.o: /home/alanau/work/mipsL4/include/string.h
termio.o: /home/alanau/work/mipsL4/include/stdio.h
termio.o: /home/alanau/work/mipsL4/include/varargs.h /usr/include/termio.h
termio.o: /usr/include/sys/termio.h /usr/include/sys/termios.h
termio.o: /usr/include/sys/feature_tests.h /usr/include/sys/ttydev.h
termio.o: /usr/include/sys/types.h /usr/include/sys/isa_defs.h
termio.o: /usr/include/sys/machtypes.h /usr/include/sys/select.h
termio.o: /usr/include/sys/time.h /usr/include/time.h
terms.o: /home/alanau/work/mipsL4/include/stdio.h
terms.o: /home/alanau/work/mipsL4/include/varargs.h /usr/include/termio.h
terms.o: /usr/include/sys/termio.h /usr/include/sys/termios.h
terms.o: /usr/include/sys/feature_tests.h /usr/include/sys/ttydev.h
terms.o: /usr/include/sys/types.h /usr/include/sys/isa_defs.h
terms.o: /usr/include/sys/machtypes.h /usr/include/sys/select.h
terms.o: /usr/include/sys/time.h /usr/include/time.h
toupper.o: /home/alanau/work/mipsL4/include/string.h
ungetc.o: /home/alanau/work/mipsL4/include/stdio.h
ungetc.o: /home/alanau/work/mipsL4/include/varargs.h
vfprintf.o: /home/alanau/work/mipsL4/include/stdarg.h
vfprintf.o: /home/alanau/work/mipsL4/include/stdio.h
vfprintf.o: /home/alanau/work/mipsL4/include/varargs.h
vsprintf.o: /home/alanau/work/mipsL4/include/stdarg.h
vsprintf.o: /home/alanau/work/mipsL4/include/stdio.h
vsprintf.o: /home/alanau/work/mipsL4/include/varargs.h
vsprintf.o: /home/alanau/work/mipsL4/include/string.h
write.o: /usr/include/termio.h /usr/include/sys/termio.h
write.o: /usr/include/sys/termios.h /usr/include/sys/feature_tests.h
write.o: /usr/include/sys/ttydev.h /usr/include/sys/types.h
write.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
write.o: /usr/include/sys/select.h /usr/include/sys/time.h
write.o: /usr/include/time.h
