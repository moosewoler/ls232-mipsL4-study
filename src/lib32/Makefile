# $Id: Makefile,v 1.3 1999/03/08 07:33:19 gernot Exp $
include ../Makefile.conf
INCLUDE=-I../include
LIB=$(TOP)/lib32

CC=$(CC32)
CFLAGS=$(CFLAGS32)
ABI_OPT= $(ABI_OPT32)
C_ABI_OPT= $(C_ABI_OPT32)
LD_ABI_OPT= $(LD_ABI_OPT32)

ASFLAGS=$(ASFLAGS32)
AS=$(AS32)

AR=$(AR32)

SUBDIRS=libc libl4

OBJECTS=
CRT0=crt0.o
CRTS=crtS.o


all: $(LIB) $(LIB)/$(CRT0) $(LIB)/$(CRTS)
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making all in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' all)  || exit;\
	done

$(LIB)/$(CRT0): $(CRT0)
	cp  $(CRT0) $(LIB)/$(CRT0)

$(LIB)/$(CRTS): $(CRTS)
	cp  $(CRTS) $(LIB)/$(CRTS)


$(LIB):
	mkdir $(LIB)

subdirs:
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making all in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' all)  || exit;\
	done

clean:
	rm -f *.o
	rm -rf $(LIB)
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making clean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' clean)  || exit;\
	done

realclean: 
	rm -f *.o  *.bak *~
	-rm -rf $(INSTALL)/lib32
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making realclean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' realclean)  || exit;\
	done
	rm -rf $(LIB)

depend:
	makedepend $(INCLUDE) *.S
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making depend in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' depend)  || exit;\
	done


install:
	-mkdir -p $(INSTALL)/lib32
	cp $(LIB)/$(CRT0) $(LIB)/$(CRTS) $(INSTALL)/lib32
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making install in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' install)  || exit;\
	done

#		# DO NOT DELETE

boot.o: ../../include/regdef.h ../../include/p4000i_board.h
boot.o: ../../include/r4kc0.h
# DO NOT DELETE

crt0.o: ../include/asm.h ../include/regdef.h
crtS.o: ../include/asm.h ../include/regdef.h
