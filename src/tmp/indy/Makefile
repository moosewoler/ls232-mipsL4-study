M4=m4

CC=cc
CFLAGS= -64 -O2 -fullwarn -woff 1209 -ansi -G 0 -mips3 -non_shared -nostdinc $(INCLUDE)

INCLUDE=-I../../include
TOOLBIN=../../../bin
ASFLAGS=-64 -fullwarn -mips3 -non_shared  $(INCLUDE)
AS=as

#LINKADDR= a0020000 #32 bit uncached
#LINKADDR= ffffffffa0020000 # uncached
#LINKADDR= ffffffff80020000 #cached
LINKADDR= ffffffff88020000 # cached, leave room for stack and kernel base
LDFLAGS=-N -64 -mips3 -non_shared -nostdlib -L$(LIB) -T $(LINKADDR) -e __boot 

TARGETS=kernel64 mk_image
OBJECTS=boot.o test.o decode.o

.SUFFIXES: .c .o .ms .s

all: $(TARGETS)

.s.o:
	$(AS) $(ASFLAGS) -o $*.o $*.s
.c.o:
	$(CC) $(CFLAGS) -c $*.c
.ms.o:
	$(M4) $*.ms > m4out.s && $(AS) $(ASFLAGS) -o $*.o m4out.s && rm m4out.s

kernel64: $(OBJECTS) $(KERNELLIBS)
	ld $(LDFLAGS) -o kernel64 $(OBJECTS) $(LIBS)

mk_image:
	$(TOOLBIN)/dit -b kernel64 kernel
	rcp kernel dazed:/usr/local/boot/kernel


clean:
	rm -f *.o kernel64 kernel

realclean:
	rm -f *.o m4out.s kernel64 kernel *.bak *~

dep:
	makedepend $(INCLUDE) *.c *.ms *.s

# DO NOT DELETE

debugger.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
debugger.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
debugger.o: ../../include/kernel/kutils.h
gpt.o: ../../include/r4kc0.h ../../include/kernel/gptdefs.h
gpt.o: ../../include/kernel/types.h ../../include/kernel/panic.h
gpt.o: ../../include/kernel/kmem.h ../../include/kernel/gptdefs.h
gpt.o: ../../include/kernel/trace.h ../../include/kernel/kutils.h
gpt.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
gpt.o: ../../include/l4/types.h
kmem.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
kmem.o: ../../include/kernel/kmem.h ../../include/kernel/gptdefs.h
kmem.o: ../../include/kernel/panic.h ../../include/kernel/trace.h
kmem.o: ../../include/r4kc0.h
schedule.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
schedule.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
schedule.o: ../../include/kernel/panic.h ../../include/kernel/trace.h
sigma0.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
sigma0.o: ../../include/kernel/panic.h ../../include/kernel/kmem.h
sigma0.o: ../../include/kernel/gptdefs.h ../../include/r4kc0.h
sigma0.o: ../../include/kernel/trace.h ../../include/kernel/kutils.h
sigma0.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
sigma0.o: ../../include/kernel/dit.h ../../include/l4/types.h
sigma0.o: ../../include/l4/ipc.h ../../include/l4/sigma0.h
tcb.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
tcb.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
tcb.o: ../../include/kernel/panic.h ../../include/kernel/kmem.h
tcb.o: ../../include/kernel/gptdefs.h ../../include/r4kc0.h
exc.o: ../../include/asm.h ../../include/regdef.h ../../include/r4kc0.h
exc.o: ../../include/kernel/panic.h ../../include/kernel/p4000i.h
exc.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
exc.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
exc.o: ../../include/l4/types.h ../../include/l4/syscalls.h
exc.o: ../../include/l4/ipc.h
gpt_f64lookup.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
gpt_f64lookup.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
gpt_f64lookup.o: ../../include/regdef.h ../../include/asm.h
gpt_f64lookup.o: ../../include/r4kc0.h ../../include/kernel/p4000i.h
interrupts.o: ../../include/asm.h ../../include/regdef.h
interrupts.o: ../../include/r4kc0.h ../../include/kernel/p4000i.h
interrupts.o: ../../include/kernel/panic.h ../../include/kernel/kernel.h
interrupts.o: ../../include/kernel/types.h ../../include/kernel/gptdefs.h
interrupts.o: ../../include/kernel/types.h
startup.o: ../../include/regdef.h ../../include/kernel/p4000i.h
startup.o: ../../include/r4kc0.h ../../include/asm.h
startup.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
startup.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
startup.o: ../../include/kernel/dit.h ../../include/l4/syscalls.h
startup.o: ../../include/l4/types.h ../../include/l4/sigma0.h
syscalls.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
syscalls.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
syscalls.o: ../../include/kernel/panic.h ../../include/asm.h
syscalls.o: ../../include/regdef.h ../../include/r4kc0.h
syscalls.o: ../../include/kernel/p4000i.h ../../include/l4/syscalls.h
syscalls.o: ../../include/l4/types.h ../../include/l4/ipc.h
tlb2.o: ../../include/asm.h ../../include/regdef.h ../../include/r4kc0.h
tlb2.o: ../../include/kernel/kernel.h ../../include/kernel/types.h
tlb2.o: ../../include/kernel/gptdefs.h ../../include/kernel/types.h
cache_err.o: ../../include/regdef.h ../../include/kernel/p4000i.h
cache_err.o: ../../include/r4kc0.h ../../include/asm.h
panic.o: ../../include/asm.h ../../include/regdef.h ../../include/r4kc0.h
panic.o: ../../include/kernel/p4000i.h ../../include/kernel/kernel.h
panic.o: ../../include/kernel/types.h ../../include/kernel/gptdefs.h
panic.o: ../../include/kernel/types.h
