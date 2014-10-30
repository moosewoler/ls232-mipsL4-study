# $Id: Makefile,v 1.38 1999/11/06 10:19:46 gernot Exp $
include ../../Makefile.conf
#ABI=-n32
DITFLAG=-a

ifeq ($(TOOL_CHAIN),GNU)
CFLAGS= -fno-unroll-loops $(USERCFLAGS)
else
CFLAGS= -OPT:unroll_size=0 $(USERCFLAGS)
endif
ASFLAGS= $(USERASFLAGS)
LDFLAGS=$(USERLDFLAGS)
LIBS=$(USERLIBS)
CRT0=$(USERCRT0)
CRTS=$(LIB)/crtS.o

FAKETARGETS=  $(TESTTARGETS)
TARGETS=serial_test serial_test2 serial_test3 read ex_reg ipc_test_client \
	ipc_test_server tlb_test mem_test new_task l4_test \
	l4_test_child tt1 tt2 tt3 time reg_test map_test
READPROG=read_test
READOBJ=read.o
T0PROG=serial_test
T0OBJ=serial_test.o
T1PROG=serial_test2
T1OBJ=serial_test2.o
T2PROG=serial_test3
T2OBJ=serial_test3.o
TIMEPROG=time_test
TIMEOBJ=time.o
EXPROG=ex_reg
EXOBJ=ex_reg.o
TLBPROG=tlb_test
TLBOBJ=tlb_test.o
ICPROG=ipc_test_client
ICOBJ=ipc_test_client.o ipc_test_client_asm.o ipc_test_timestamp.o
ISPROG=ipc_test_server
ISOBJ=ipc_test_server.o ipc_test_server_asm.o
MPROG=mem_test
MOBJ=mem_test.o
NTOBJ=new_task.o
NTPROG=new_task

L4TPROG=l4_test
L4TOBJ=l4_test.o chief_ipc.o
#L4TPROG=intr_test
#L4TOBJ=intr_test.o
L4TCPROG=l4_test_child
L4TCOBJ=l4_test_child.o
REGPROG=reg_test
REGOBJ=reg_main.o reg_subthread.o reg_pagefaulter.o
MAPMAINOBJ=map_main.o
MAPMAINPROG=map_main
MAPCHILDOBJ=map_child.o
MAPCHILDPROG=map_child
MAPGCHILDOBJ=map_gchild.o
MAPGCHILDPROG=map_gchild

all:	kernel $(FAKETARGETS)

kernel:	../kernel
	cp ../kernel .

ipc_test:  ipc_test_kernel its itc nt
ipc_test_kernel:	kernel
	cp -p kernel kernel.ipc_test

itc: $(ICOBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.ipc_test` -o $(ICPROG) \
		$(CRTS) $(ICOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(ICPROG) kernel.ipc_test

its: $(ISOBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.ipc_test` -o $(ISPROG) \
		$(CRTS) $(ISOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(ISPROG) kernel.ipc_test

nt: $(NTOBJ)
	$(LD)  $(LDFLAGS) -T 0x80000 -o $(NTPROG) $(CRT0)  $(NTOBJ) $(LIBS)
	$(TOOLBIN)/dit -n -a $(NTPROG) kernel.ipc_test

l4_tester:  l4_tester_kernel l4t l4tc test1 test2 test3
l4_tester_kernel:	kernel
	cp -p kernel kernel.l4_tester

test1: tt1.o
	$(LD)  $(LDFLAGS) -T 0x88000 -o tt1 $(CRT0)  tt1.o $(LIBS)
	$(TOOLBIN)/dit -n -a tt1 kernel.l4_tester

test2: tt2.o
	$(LD)  $(LDFLAGS) -T 0x90000 -o tt2 $(CRT0)  tt2.o $(LIBS)
	$(TOOLBIN)/dit -n -a tt2 kernel.l4_tester

test3: tt3.o
	$(LD)  $(LDFLAGS) -T 0x98000 -o tt3 $(CRT0)  tt3.o $(LIBS)
	$(TOOLBIN)/dit -n -a tt3 kernel.l4_tester


l4tc: $(L4TCOBJ)
	$(LD)  $(LDFLAGS) -T 0x80000 -o $(L4TCPROG) $(L4TCOBJ) $(LIBS) $(CRT0)
	$(TOOLBIN)/dit -n -a $(L4TCPROG) kernel.l4_tester


l4t: $(L4TOBJ)
	$(LD) $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.l4_tester` -o $(L4TPROG) \
		$(CRTS) $(L4TOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(L4TPROG) kernel.l4_tester


mem:	$(MOBJ)	kernel
	cp -p kernel kernel.mem
	$(LD) $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.mem` -o $(MPROG) $(CRTS) \
		$(MOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(MPROG) kernel.mem

t0: $(T0OBJ)	kernel
	cp -p kernel kernel.t0
	$(LD) $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.t0` -o $(T0PROG) $(CRTS) \
		$(T0OBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(T0PROG) kernel.t0

t1: $(T1OBJ)	kernel
	cp -p kernel kernel.t1
	$(LD) $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.t1` -o $(T1PROG) $(CRTS) \
		$(T1OBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(T1PROG) kernel.t1

t2: $(T2OBJ)	kernel
	cp -p kernel kernel.t2
	$(LD) $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.t2` -o $(T2PROG) $(CRTS) \
		$(T2OBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(T2PROG) kernel.t2

read: $(READOBJ)	kernel
	cp -p kernel kernel.read
	$(LD) $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.read` -o $(READPROG) \
	$(CRTS) $(READOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(READPROG) kernel.read

time: $(TIMEOBJ)	kernel
	cp -p kernel kernel.time
	$(LD) $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.time` -o $(TIMEPROG) \
	$(CRTS) $(TIMEOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(TIMEPROG) kernel.time

tlb: $(TLBOBJ)	kernel
	cp -p kernel kernel.tlb
	$(LD) $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.tlb` -o $(TLBPROG) \
		$(CRTS) $(TLBOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(TLBPROG) kernel.tlb

ex: $(EXOBJ)	kernel
	cp -p kernel kernel.ex
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.ex` -o $(EXPROG) \
		$(CRTS) $(EXOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(EXPROG) kernel.ex

reg_test:  reg_test_kernel reg_tester
reg_test_kernel:	kernel
	cp -p kernel kernel.reg_test
reg_tester:	$(REGOBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.reg_test` -o $(REGPROG) \
		$(CRTS) $(REGOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(REGPROG) kernel.reg_test

map_test:  map_test_kernel map_m map_ch map_gch
map_test_kernel:	kernel
	cp -p kernel kernel.map_test

map_m: $(MAPMAINOBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.map_test` \
		-o $(MAPMAINPROG) $(CRTS) $(MAPMAINOBJ) $(LIBS)
	$(TOOLBIN)/dit $(DITFLAG) $(MAPMAINPROG) kernel.map_test

map_ch: $(MAPCHILDOBJ)
	$(LD)  $(LDFLAGS) -T 0x80000 \
		-o $(MAPCHILDPROG) $(CRT0) $(MAPCHILDOBJ) $(LIBS)
	$(TOOLBIN)/dit -n $(DITFLAG) $(MAPCHILDPROG) kernel.map_test

map_gch: $(MAPGCHILDOBJ)
	$(LD)  $(LDFLAGS) -T `$(TOOLBIN)/dit -l kernel.map_test` \
		-o $(MAPGCHILDPROG) $(CRT0) $(MAPGCHILDOBJ) $(LIBS)
	$(TOOLBIN)/dit -n $(DITFLAG) $(MAPGCHILDPROG) kernel.map_test

#mun:
#	$(TOOLBIN)/dit $(DITFLAG) tmp/mungi kernel
#	$(TOOLBIN)/dit -n $(DITFLAG) tmp/spare kernel
#	$(TOOLBIN)/dit -n $(DITFLAG) tmp/test kernel


install:
	for i in $(FAKETARGETS); do \
		cp -p kernel.$$i $(BOOT); \
		chmod a+r $(BOOT)/kernel.$$i; \
	done


clean:
	-rm *.o kernel* $(TARGETS)

realclean:	clean
	-rm -f *.bak *~

depend:
	-makedepend $(INCLUDE) *.S
	-makedepend $(INCLUDE) *.c

# DO NOT DELETE

ex_reg.o: /home/gernot/work/mipsL4/include/libc.h
ex_reg.o: /home/gernot/work/mipsL4/include/assert.h
ex_reg.o: /home/gernot/work/mipsL4/include/l4/ipc.h
ex_reg.o: /home/gernot/work/mipsL4/include/l4/types.h
ex_reg.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
intr_test.o: /home/gernot/work/mipsL4/include/libc.h
intr_test.o: /home/gernot/work/mipsL4/include/assert.h
intr_test.o: /home/gernot/work/mipsL4/include/l4/ipc.h
intr_test.o: /home/gernot/work/mipsL4/include/l4/types.h
intr_test.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
intr_test.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
intr_test.o: /home/gernot/work/mipsL4/include/kernel/machine.h
ipc_test_client.o: /home/gernot/work/mipsL4/include/libc.h
ipc_test_client.o: /home/gernot/work/mipsL4/include/assert.h
ipc_test_client.o: /home/gernot/work/mipsL4/include/l4/ipc.h
ipc_test_client.o: /home/gernot/work/mipsL4/include/l4/types.h
ipc_test_client.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
ipc_test_client.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
ipc_test_server.o: /home/gernot/work/mipsL4/include/libc.h
ipc_test_server.o: /home/gernot/work/mipsL4/include/assert.h
ipc_test_server.o: /home/gernot/work/mipsL4/include/l4/ipc.h
ipc_test_server.o: /home/gernot/work/mipsL4/include/l4/types.h
ipc_test_server.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/string.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/types.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/libc.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/assert.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/l4/ipc.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/l4/types.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/kernel/machine.h
ipc_test_timestamp.o: /home/gernot/work/mipsL4/include/kernel/gt64010a.h
l4_test.o: /home/gernot/work/mipsL4/include/libc.h
l4_test.o: /home/gernot/work/mipsL4/include/string.h
l4_test.o: /home/gernot/work/mipsL4/include/types.h
l4_test.o: /home/gernot/work/mipsL4/include/assert.h
l4_test.o: /home/gernot/work/mipsL4/include/l4/ipc.h
l4_test.o: /home/gernot/work/mipsL4/include/l4/types.h
l4_test.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
l4_test.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
l4_test_child.o: /home/gernot/work/mipsL4/include/libc.h
l4_test_child.o: /home/gernot/work/mipsL4/include/string.h
l4_test_child.o: /home/gernot/work/mipsL4/include/types.h
l4_test_child.o: /home/gernot/work/mipsL4/include/assert.h
l4_test_child.o: /home/gernot/work/mipsL4/include/l4/ipc.h
l4_test_child.o: /home/gernot/work/mipsL4/include/l4/types.h
l4_test_child.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
l4_test_child.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
mem_test.o: /home/gernot/work/mipsL4/include/libc.h
mem_test.o: /home/gernot/work/mipsL4/include/assert.h
mem_test.o: /home/gernot/work/mipsL4/include/l4/ipc.h
mem_test.o: /home/gernot/work/mipsL4/include/l4/types.h
mem_test.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
mem_test.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
read.o: /home/gernot/work/mipsL4/include/l4/ipc.h
read.o: /home/gernot/work/mipsL4/include/l4/types.h
read.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
read.o: /home/gernot/work/mipsL4/include/libc.h
read.o: /home/gernot/work/mipsL4/include/assert.h
serial_test.o: /home/gernot/work/mipsL4/include/libc.h
serial_test.o: /home/gernot/work/mipsL4/include/l4/ipc.h
serial_test.o: /home/gernot/work/mipsL4/include/l4/types.h
serial_test.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
serial_test2.o: /home/gernot/work/mipsL4/include/libc.h
serial_test2.o: /home/gernot/work/mipsL4/include/l4/ipc.h
serial_test2.o: /home/gernot/work/mipsL4/include/l4/types.h
serial_test2.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
serial_test3.o: /home/gernot/work/mipsL4/include/libc.h
serial_test3.o: /home/gernot/work/mipsL4/include/assert.h
serial_test3.o: /home/gernot/work/mipsL4/include/l4/ipc.h
serial_test3.o: /home/gernot/work/mipsL4/include/l4/types.h
serial_test3.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
serial_test3.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
time.o: /home/gernot/work/mipsL4/include/libc.h
time.o: /home/gernot/work/mipsL4/include/l4/time.h
time.o: /home/gernot/work/mipsL4/include/l4/types.h
time.o: /home/gernot/work/mipsL4/include/l4/ipc.h
time.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
time.o: /home/gernot/work/mipsL4/include/assert.h
tlb_test.o: /home/gernot/work/mipsL4/include/libc.h
tlb_test.o: /home/gernot/work/mipsL4/include/l4/ipc.h
tlb_test.o: /home/gernot/work/mipsL4/include/l4/types.h
tlb_test.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
tlb_test.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
tt1.o: /home/gernot/work/mipsL4/include/libc.h
tt1.o: /home/gernot/work/mipsL4/include/assert.h
tt1.o: /home/gernot/work/mipsL4/include/l4/ipc.h
tt1.o: /home/gernot/work/mipsL4/include/l4/types.h
tt1.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
tt1.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
tt2.o: /home/gernot/work/mipsL4/include/libc.h
tt2.o: /home/gernot/work/mipsL4/include/assert.h
tt2.o: /home/gernot/work/mipsL4/include/l4/ipc.h
tt2.o: /home/gernot/work/mipsL4/include/l4/types.h
tt2.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
tt2.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
tt3.o: /home/gernot/work/mipsL4/include/libc.h
tt3.o: /home/gernot/work/mipsL4/include/assert.h
tt3.o: /home/gernot/work/mipsL4/include/l4/ipc.h
tt3.o: /home/gernot/work/mipsL4/include/l4/types.h
tt3.o: /home/gernot/work/mipsL4/include/l4/syscalls.h
tt3.o: /home/gernot/work/mipsL4/include/l4/sigma0.h
