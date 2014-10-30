# $Id: Makefile,v 1.27 1999/02/10 04:56:38 gernot Exp $
include ../Makefile.conf
INCLUDE=-I../include
LIB=$(TOP)/lib


CC=cc
CFLAGS=$(USERCFLAGS)

ASFLAGS=$(USERASFLAGS)

SUBDIRS=libc libl4

OBJECTS=
CRT0=crt0.o
CRTS=crtS.o


all: 	$(LIB) $(LIB)/$(CRT0) $(LIB)/$(CRTS)
	@for i in $(SUBDIRS);\
	do \
	(cd $$i; echo "making all in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' all)  || exit;\
	done

$(LIB)/$(CRT0): $(CRT0)
	cp $(CRT0) $(LIB)/$(CRT0)

$(LIB)/$(CRTS): $(CRTS)
	cp $(CRTS) $(LIB)/$(CRTS)


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
	rm -rf $(LIB)

realclean: 
	rm -f *.o  *.bak *~
	-rm -rf $(INSTALL)/lib
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
	-mkdir -p $(INSTALL)/lib
	cp $(LIB)/$(CRT0) $(LIB)/$(CRTS) $(INSTALL)/lib
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
