# $Id: Makefile,v 1.44 1999/03/22 04:45:49 gernot Exp $
include ../Makefile.conf

KERNEL=$(INSTALL)/kernel
LIB=$(TOP)/lib


CFLAGS= $(KCFLAGS) #-D KDEBUG

ASFLAGS=$(KASFLAGS)


#LINKADDR= a0020000 #32 bit uncached
#LINKADDR= ffffffffa0020000 # uncached
#LINKADDR= ffffffff80020000 #cached



LIBS= $(KERNELLIBS) -ll4
LDFLAGS=-mips3 $(LD_ABI_OPT) -T $(LINKADDR) -L$(LIB) \
	-e __boot $(MACHSUBDIR)/boot.o

PROCS=test_threads
TTOBJS=test_threads.o

TARGETS=subdirs version kernel64 mk_image post_kern
OBJECTS=exc.o startup.o interrupts.o syscalls.o \
	panic.o cache_err.o schedule.o kmem.o asid.o tlb.o\
	debugger.o sigma0.o ipc.o version.o tcb.o 
SUBDIRS=libkern $(MACHSUBDIR) $(VM_CODE)
DEVS=drivers
OTHERDIRS=$(DEVS) #test

all:	$(TARGETS)

kernel64: $(OBJECTS) $(KERNELLIBS)
	$(LD) $(LDFLAGS) -o kernel64 $(OBJECTS) $(LIBS)

mk_image:
	$(TOOLBIN)/dit -i kernel64 kernel

install: kernel64
	-mkdir -p $(KERNEL)
	$(TOOLBIN)/dit -i kernel64 $(KERNEL)/kernel
	@for i in $(OTHERDIRS);\
	do \
	(cd $$i; echo "making install in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' install)  || exit;\
	done
#	rcp kernel murungal:/tftpboot
#	rcp kernel dazed:/tftpboot/kernel.kev
#	rcp $(TOP)/kernel/kernel theremin:/tftpboot/kernel.alan

version:
	mv version.c version.bak
	awk -f newvers 	< version.bak >version.c


subdirs:
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making all in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' all)  || exit;\
	done
post_kern:
	@for i in $(OTHERDIRS) ;\
	do \
	(cd $$i;  echo "making all in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' all)  || exit;\
	done

clean:
	rm -f *.o kernel*
	@for i in $(SUBDIRS) $(OTHERDIRS);\
	do \
	(cd $$i; echo "making clean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' clean)  || exit;\
	done

realclean:
	rm -f *.o m4out.S kernel* *.bak *~
	-rm -rf $(KERNEL)
	@for i in $(SUBDIRS) $(OTHERDIRS) test ;\
	do \
	(cd $$i; echo "making realclean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' realclean)  || exit;\
	done

depend:
	makedepend $(INCLUDE) *.c *.S
	@for i in $(SUBDIRS) $(OTHERDIRS) test ;\
	do \
	(cd $$i; echo "making depend in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' depend)  || exit;\
	done

# DO NOT DELETE

debugger.o: /home/alanau/mipsL4/include/kernel/kernel.h
debugger.o: /home/alanau/mipsL4/include/kernel/types.h
debugger.o: /home/alanau/mipsL4/include/l4/types.h
debugger.o: /home/alanau/mipsL4/include/kernel/kutils.h
debugger.o: /home/alanau/mipsL4/include/kernel/vm.h
debugger.o: /home/alanau/mipsL4/include/l4/sigma0.h
debugger.o: /home/alanau/mipsL4/include/alloca.h
debugger.o: /home/alanau/mipsL4/include/types.h
debugger.o: /home/alanau/mipsL4/include/r4kc0.h
ipc.o: /home/alanau/mipsL4/include/kernel/kernel.h
ipc.o: /home/alanau/mipsL4/include/kernel/types.h
ipc.o: /home/alanau/mipsL4/include/l4/types.h
ipc.o: /home/alanau/mipsL4/include/kernel/vm.h
ipc.o: /home/alanau/mipsL4/include/kernel/panic.h
ipc.o: /home/alanau/mipsL4/include/l4/ipc.h
kmem.o: /home/alanau/mipsL4/include/kernel/kmem.h
kmem.o: /home/alanau/mipsL4/include/types.h
kmem.o: /home/alanau/mipsL4/include/kernel/panic.h
kmem.o: /home/alanau/mipsL4/include/kernel/trace.h
kmem.o: /home/alanau/mipsL4/include/kernel/kernel.h
kmem.o: /home/alanau/mipsL4/include/kernel/types.h
kmem.o: /home/alanau/mipsL4/include/l4/types.h
kmem.o: /home/alanau/mipsL4/include/r4kc0.h
schedule.o: /home/alanau/mipsL4/include/kernel/kernel.h
schedule.o: /home/alanau/mipsL4/include/kernel/types.h
schedule.o: /home/alanau/mipsL4/include/l4/types.h
schedule.o: /home/alanau/mipsL4/include/kernel/panic.h
schedule.o: /home/alanau/mipsL4/include/kernel/trace.h
schedule.o: /home/alanau/mipsL4/include/stdio.h
schedule.o: /home/alanau/mipsL4/include/varargs.h
sigma0.o: /home/alanau/mipsL4/include/kernel/panic.h
sigma0.o: /home/alanau/mipsL4/include/kernel/kmem.h
sigma0.o: /home/alanau/mipsL4/include/types.h
sigma0.o: /home/alanau/mipsL4/include/kernel/vm.h
sigma0.o: /home/alanau/mipsL4/include/r4kc0.h
sigma0.o: /home/alanau/mipsL4/include/kernel/trace.h
sigma0.o: /home/alanau/mipsL4/include/kernel/kutils.h
sigma0.o: /home/alanau/mipsL4/include/kernel/kernel.h
sigma0.o: /home/alanau/mipsL4/include/kernel/types.h
sigma0.o: /home/alanau/mipsL4/include/l4/types.h
sigma0.o: /home/alanau/mipsL4/include/kernel/dit.h
sigma0.o: /home/alanau/mipsL4/include/l4/ipc.h
sigma0.o: /home/alanau/mipsL4/include/l4/syscalls.h
sigma0.o: /home/alanau/mipsL4/include/l4/sigma0.h
tcb.o: /home/alanau/mipsL4/include/kernel/kernel.h
tcb.o: /home/alanau/mipsL4/include/kernel/types.h
tcb.o: /home/alanau/mipsL4/include/l4/types.h
tcb.o: /home/alanau/mipsL4/include/kernel/panic.h
tcb.o: /home/alanau/mipsL4/include/kernel/kmem.h
tcb.o: /home/alanau/mipsL4/include/types.h
tcb.o: /home/alanau/mipsL4/include/r4kc0.h
version.o: /home/alanau/mipsL4/include/l4/types.h
version.o: /home/alanau/mipsL4/include/kernel/kernel.h
version.o: /home/alanau/mipsL4/include/kernel/types.h
asid.o: /home/alanau/mipsL4/include/asm.h
asid.o: /home/alanau/mipsL4/include/regdef.h
asid.o: /home/alanau/mipsL4/include/r4kc0.h
asid.o: /home/alanau/mipsL4/include/kernel/kernel.h
asid.o: /home/alanau/mipsL4/include/kernel/types.h
asid.o: /home/alanau/mipsL4/include/l4/types.h
asid.o: /home/alanau/mipsL4/include/kernel/machine.h macros.h
cache_err.o: /home/alanau/mipsL4/include/regdef.h
cache_err.o: /home/alanau/mipsL4/include/kernel/machine.h
cache_err.o: /home/alanau/mipsL4/include/kernel/kernel.h
cache_err.o: /home/alanau/mipsL4/include/kernel/types.h
cache_err.o: /home/alanau/mipsL4/include/l4/types.h
cache_err.o: /home/alanau/mipsL4/include/r4kc0.h
cache_err.o: /home/alanau/mipsL4/include/asm.h
exc.o: /home/alanau/mipsL4/include/asm.h /home/alanau/mipsL4/include/regdef.h
exc.o: /home/alanau/mipsL4/include/r4kc0.h
exc.o: /home/alanau/mipsL4/include/kernel/panic.h
exc.o: /home/alanau/mipsL4/include/kernel/machine.h
exc.o: /home/alanau/mipsL4/include/kernel/kernel.h
exc.o: /home/alanau/mipsL4/include/kernel/types.h
exc.o: /home/alanau/mipsL4/include/l4/types.h
exc.o: /home/alanau/mipsL4/include/l4/syscalls.h
exc.o: /home/alanau/mipsL4/include/l4/ipc.h macros.h
interrupts.o: /home/alanau/mipsL4/include/asm.h
interrupts.o: /home/alanau/mipsL4/include/regdef.h
interrupts.o: /home/alanau/mipsL4/include/l4/ipc.h
interrupts.o: /home/alanau/mipsL4/include/l4/types.h
interrupts.o: /home/alanau/mipsL4/include/l4/sigma0.h
interrupts.o: /home/alanau/mipsL4/include/r4kc0.h
interrupts.o: /home/alanau/mipsL4/include/kernel/machine.h
interrupts.o: /home/alanau/mipsL4/include/kernel/panic.h
interrupts.o: /home/alanau/mipsL4/include/kernel/kernel.h
interrupts.o: /home/alanau/mipsL4/include/kernel/types.h macros.h
panic.o: /home/alanau/mipsL4/include/asm.h
panic.o: /home/alanau/mipsL4/include/regdef.h
panic.o: /home/alanau/mipsL4/include/r4kc0.h
panic.o: /home/alanau/mipsL4/include/kernel/machine.h
panic.o: /home/alanau/mipsL4/include/kernel/kernel.h
panic.o: /home/alanau/mipsL4/include/kernel/types.h
panic.o: /home/alanau/mipsL4/include/l4/types.h
startup.o: /home/alanau/mipsL4/include/regdef.h
startup.o: /home/alanau/mipsL4/include/kernel/machine.h
startup.o: /home/alanau/mipsL4/include/r4kc0.h
startup.o: /home/alanau/mipsL4/include/asm.h
startup.o: /home/alanau/mipsL4/include/kernel/kernel.h
startup.o: /home/alanau/mipsL4/include/kernel/types.h
startup.o: /home/alanau/mipsL4/include/l4/types.h
startup.o: /home/alanau/mipsL4/include/kernel/dit.h
startup.o: /home/alanau/mipsL4/include/l4/syscalls.h
startup.o: /home/alanau/mipsL4/include/l4/sigma0.h macros.h
syscalls.o: /home/alanau/mipsL4/include/kernel/kernel.h
syscalls.o: /home/alanau/mipsL4/include/kernel/types.h
syscalls.o: /home/alanau/mipsL4/include/l4/types.h
syscalls.o: /home/alanau/mipsL4/include/kernel/panic.h
syscalls.o: /home/alanau/mipsL4/include/asm.h
syscalls.o: /home/alanau/mipsL4/include/regdef.h
syscalls.o: /home/alanau/mipsL4/include/r4kc0.h
syscalls.o: /home/alanau/mipsL4/include/kernel/machine.h
syscalls.o: /home/alanau/mipsL4/include/l4/syscalls.h
syscalls.o: /home/alanau/mipsL4/include/l4/ipc.h macros.h
tlb.o: /home/alanau/mipsL4/include/asm.h /home/alanau/mipsL4/include/regdef.h
tlb.o: /home/alanau/mipsL4/include/r4kc0.h
tlb.o: /home/alanau/mipsL4/include/kernel/kernel.h
tlb.o: /home/alanau/mipsL4/include/kernel/types.h
tlb.o: /home/alanau/mipsL4/include/l4/types.h
tlb.o: /home/alanau/mipsL4/include/kernel/machine.h
tlb.o: /home/alanau/mipsL4/include/kernel/vm.h
