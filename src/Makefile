CURRENT_DIR=.
TOP=/usr/local/mipsL4
include ../src/Makefile.conf

SUBDIRS= include lib kernel
TEST= kernel/test


all:	$(TOP)
	@for i in $(SUBDIRS) ;\
	do \
	(cd $$i; echo "making all in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' all)  || exit;\
	done

$(TOP):
	-mkdir -p $(TOP)

depend:
	@for i in $(SUBDIRS) $(TEST) ../example ../doc tools;\
	do \
	(cd $$i; echo "making depend in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' depend)  || exit;\
	done

clean:	tools-clean
	@for i in $(SUBDIRS) $(TEST) ../example ../doc;\
	do \
	(cd $$i; echo "making clean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' clean)  || exit;\
	done

realclean: tools-realclean
	@for i in $(SUBDIRS) $(TEST) ../example ../doc;\
	do \
	(cd $$i; echo "making realclean in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' realclean)  || exit;\
	done
	rm -f *~ *.bak
	-rm -rf $(TOP)/kernel

install:
	rm -f $(KERNEL)
	@for i in $(SUBDIRS) ../example ../doc;\
	do \
	(cd $$i; echo "making install in $(CURRENT_DIR)/$$i."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
	'TOP=$(TOP)' install)  || exit;\
	done

tools: tools-all

tools-clean:
	@(cd tools; echo "making clean in $(CURRENT_DIR)/tools."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/tools" \
	'TOP=$(TOP)' clean) 

tools-realclean:
	@(cd tools; echo "making realclean in $(CURRENT_DIR)/tools."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/tools" \
	'TOP=$(TOP)' realclean) 

tools-all:
	@(cd tools; echo "making all in $(CURRENT_DIR)/tools."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/tools" \
	'TOP=$(TOP)' all) 

tools-install:
	@(cd tools; echo "making install in $(CURRENT_DIR)/tools."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/tools" \
	'TOP=$(TOP)' install)

doc:
	@(cd ../doc; echo "making all in $(CURRENT_DIR)/../doc."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/../doc" \
	'TOP=$(TOP)' all)

doc-install:
	@(cd ../doc; echo "making install in $(CURRENT_DIR)/../doc."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/../doc" \
	'TOP=$(TOP)' install)

doc-depend:
	@(cd ../doc; echo "making depend in $(CURRENT_DIR)/../doc."; \
	$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/../doc" \
	'TOP=$(TOP)' depend)



test:
	@(cd $(TEST); \
		echo "making tests in $(CURRENT_DIR)/$(TEST)."; \
		$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$(TEST)" \
		'TOP=$(TOP)' all) ;

test-install:
	@(cd $(TEST); \
		echo "making install in $(CURRENT_DIR)/$(TEST)."; \
		$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$(TEST)" \
		'TOP=$(TOP)' install) ;
