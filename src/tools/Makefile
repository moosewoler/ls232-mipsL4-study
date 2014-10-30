# $Id: Makefile,v 1.10 1999/07/16 04:36:21 gernot Exp $
include ../Makefile.conf
SUBDIRS=dit

all:
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making all in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' all) ;\
	done

clean:
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making clean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' clean) ;\
	done

realclean:
	rm -f *~ *.bak
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making realclean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' realclean) ;\
	done

install:
	@if [ ! -d $(TOOLBIN) ]; then  \
	echo "mkdir $(TOOLBIN)" ;\
	mkdir -p $(TOOLBIN) ;\
	fi ;\
	for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making install in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' install) ;\
	done

depend:
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making depend in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' depend) ;\
	done
