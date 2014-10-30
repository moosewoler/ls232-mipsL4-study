# $Id: Makefile,v 1.12 1999/02/10 04:54:58 gernot Exp $
include ../Makefile.conf
INCLUDE=$(TOP)/include

SUBDIRS=kernel l4

all:

depend:
	-mkdir -p $(INCLUDE)
	/bin/cp -p *.h $(INCLUDE)
	-mkdir -p $(INCLUDE)/l4
	/bin/cp -p l4/*.h $(INCLUDE)/l4
	-mkdir -p $(INCLUDE)/kernel
	/bin/cp -p kernel/*.h $(INCLUDE)/kernel

clean:
	rm -rf $(INCLUDE)
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making clean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' clean)  || exit;\
	done

realclean:
	rm -f *~ *.bak
	-rm -rf $(INCLUDE) $(INSTALL)/include
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making realclean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' realclean)  || exit;\
	done


install:
	-mkdir -p $(INSTALL)/include
	/bin/cp -p $(INCLUDE)/*.h $(INSTALL)/include
	-mkdir -p $(INSTALL)/include/l4
	/bin/cp -p $(INCLUDE)/l4/*.h $(INSTALL)/include/l4
	-mkdir -p $(INSTALL)/include/kernel
	/bin/cp -p $(INCLUDE)/kernel/*.h $(INSTALL)/include/kernel
