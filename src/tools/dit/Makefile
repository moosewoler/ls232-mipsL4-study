# $Id: Makefile,v 1.11 1999/03/12 04:40:52 gernot Exp $
include ../../Makefile.conf
CC=$(NATIVECC)
CFLAGS=$(NATIVECFLAGS)
TARGETS=dit
OBJECTS=dit.o append_data.o append_elf32.o append_elf64.o init_image.o\
	link_addr.o map.o utils.o append_ecoff.o


all:	$(TARGETS)

dit:	$(OBJECTS)
	$(CC) -o dit $(OBJECTS)

clean:
	rm -f *.o $(TARGETS)

realclean:
	rm -f *.o $(TARGETS) *.bak *~
	-cd $(TOOLBIN) && rm -rf $(TARGETS)

install:
	cp $(TARGETS) $(TOOLBIN)

depend:
	makedepend $(INCLUDE) *.c

# DO NOT DELETE

append_data.o: elf.h protos.h ../../include/kernel/dit.h
append_data.o: /home/alanau/mipsL4/include/stdio.h
append_data.o: /home/alanau/mipsL4/include/varargs.h
append_data.o: /home/alanau/mipsL4/include/string.h
append_data.o: /home/alanau/mipsL4/include/types.h /usr/include/fcntl.h
append_data.o: /usr/include/sys/fcntl.h /usr/include/standards.h
append_data.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
append_data.o: /usr/include/sys/select.h /usr/include/sys/seek.h
append_data.o: /usr/include/sys/mode.h /usr/include/stdlib.h
append_data.o: /usr/include/getopt.h /home/alanau/mipsL4/include/assert.h
append_data.o: /usr/include/sys/stat.h
append_ecoff.o: protos.h elf.h coff.h ../../include/kernel/dit.h
append_ecoff.o: /home/alanau/mipsL4/include/stdio.h
append_ecoff.o: /home/alanau/mipsL4/include/varargs.h
append_ecoff.o: /home/alanau/mipsL4/include/string.h
append_ecoff.o: /home/alanau/mipsL4/include/types.h /usr/include/fcntl.h
append_ecoff.o: /usr/include/sys/fcntl.h /usr/include/standards.h
append_ecoff.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
append_ecoff.o: /usr/include/sys/select.h /usr/include/sys/seek.h
append_ecoff.o: /usr/include/sys/mode.h /usr/include/stdlib.h
append_ecoff.o: /usr/include/getopt.h /home/alanau/mipsL4/include/assert.h
append_ecoff.o: /usr/include/sys/stat.h
append_elf32.o: elf.h protos.h ../../include/kernel/dit.h
append_elf32.o: /home/alanau/mipsL4/include/stdio.h
append_elf32.o: /home/alanau/mipsL4/include/varargs.h
append_elf32.o: /home/alanau/mipsL4/include/string.h
append_elf32.o: /home/alanau/mipsL4/include/types.h /usr/include/fcntl.h
append_elf32.o: /usr/include/sys/fcntl.h /usr/include/standards.h
append_elf32.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
append_elf32.o: /usr/include/sys/select.h /usr/include/sys/seek.h
append_elf32.o: /usr/include/sys/mode.h /usr/include/stdlib.h
append_elf32.o: /usr/include/getopt.h /home/alanau/mipsL4/include/assert.h
append_elf32.o: /usr/include/sys/stat.h
append_elf64.o: elf.h protos.h ../../include/kernel/dit.h
append_elf64.o: /home/alanau/mipsL4/include/stdio.h
append_elf64.o: /home/alanau/mipsL4/include/varargs.h
append_elf64.o: /home/alanau/mipsL4/include/string.h
append_elf64.o: /home/alanau/mipsL4/include/types.h /usr/include/fcntl.h
append_elf64.o: /usr/include/sys/fcntl.h /usr/include/standards.h
append_elf64.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
append_elf64.o: /usr/include/sys/select.h /usr/include/sys/seek.h
append_elf64.o: /usr/include/sys/mode.h /usr/include/stdlib.h
append_elf64.o: /usr/include/getopt.h /home/alanau/mipsL4/include/assert.h
append_elf64.o: /usr/include/sys/stat.h
dit.o: /home/alanau/mipsL4/include/stdio.h
dit.o: /home/alanau/mipsL4/include/varargs.h elf.h /usr/include/sys/types.h
dit.o: /usr/include/standards.h /usr/include/mach/machine/vm_types.h
dit.o: /usr/include/sys/select.h /usr/include/sys/stat.h
dit.o: /usr/include/sys/mode.h /usr/include/fcntl.h /usr/include/sys/fcntl.h
dit.o: /usr/include/sys/seek.h /usr/include/stdlib.h /usr/include/getopt.h
dit.o: /home/alanau/mipsL4/include/assert.h ../../include/kernel/dit.h
dit.o: protos.h
init_image.o: elf.h protos.h ../../include/kernel/dit.h
init_image.o: /home/alanau/mipsL4/include/stdio.h
init_image.o: /home/alanau/mipsL4/include/varargs.h /usr/include/fcntl.h
init_image.o: /usr/include/sys/fcntl.h /usr/include/standards.h
init_image.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
init_image.o: /usr/include/sys/select.h /usr/include/sys/seek.h
init_image.o: /usr/include/sys/mode.h /usr/include/stdlib.h
init_image.o: /usr/include/getopt.h /home/alanau/mipsL4/include/assert.h
init_image.o: /usr/include/sys/stat.h
link_addr.o: elf.h protos.h ../../include/kernel/dit.h
link_addr.o: /home/alanau/mipsL4/include/stdio.h
link_addr.o: /home/alanau/mipsL4/include/varargs.h /usr/include/fcntl.h
link_addr.o: /usr/include/sys/fcntl.h /usr/include/standards.h
link_addr.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
link_addr.o: /usr/include/sys/select.h /usr/include/sys/seek.h
link_addr.o: /usr/include/sys/mode.h /usr/include/stdlib.h
link_addr.o: /usr/include/getopt.h /home/alanau/mipsL4/include/assert.h
link_addr.o: /usr/include/sys/stat.h
map.o: elf.h protos.h ../../include/kernel/dit.h
map.o: /home/alanau/mipsL4/include/stdio.h
map.o: /home/alanau/mipsL4/include/varargs.h /usr/include/fcntl.h
map.o: /usr/include/sys/fcntl.h /usr/include/standards.h
map.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
map.o: /usr/include/sys/select.h /usr/include/sys/seek.h
map.o: /usr/include/sys/mode.h /usr/include/stdlib.h /usr/include/getopt.h
map.o: /home/alanau/mipsL4/include/assert.h /usr/include/sys/stat.h
utils.o: protos.h elf.h
