# $Id: Makefile,v 1.5 1998/12/31 00:28:42 gernot Exp $
include ../src/Makefile.conf

REFMAN=$(INSTALL)/doc/l4-refman.ps.gz

ROOT=l4R4x00
BIB=l4R4x00.bib
MANDIR=man
CURRENT_DIR=$(TOP)
SUBDIRS= man l4uman
L4VERSION=../src/kernel/version.c

all:		$(ROOT).ps
		@for i in $(SUBDIRS) ;\
		do \
		(cd $$i; echo "making all in $(CURRENT_DIR)/$$i."; \
		$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
		'TOP=$(TOP)' all) ;\
		done

install:	$(ROOT).ps
		-mkdir -p $(INSTALL)/doc
		gzip --best <$(ROOT).ps >$(REFMAN)
		@for i in $(SUBDIRS) ;\
		do \
		(cd $$i; echo "making install in $(CURRENT_DIR)/$$i."; \
		$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
		'TOP=$(TOP)' install) ;\
		done

$(ROOT).ps:	$(ROOT).dvi
		TEXINPUTS=l4uman/sty:$$TEXINPUTS: dvips -o $(ROOT).ps $(ROOT)

$(ROOT).dvi:	$(ROOT).tex $(BIB) l4version.tex
		TEXINPUTS=l4uman/sty:$$TEXINPUTS: latex $(ROOT)
		bibtex $(ROOT)
		TEXINPUTS=l4uman/sty:$$TEXINPUTS: latex $(ROOT)
		TEXINPUTS=l4uman/sty:$$TEXINPUTS: latex $(ROOT)

l4version.tex:	$(L4VERSION)
		sed -n < $(L4VERSION) > l4version.tex \
			'/l4_version/s/.*=\([0-9 	]*\);.*/\1/p'

clean:
		rm -f *.log *.dvi *.aux *.ps *.toc *.blg *.bbl *.cb *~
		@for i in $(SUBDIRS) ;\
		do \
		(cd $$i; echo "making clean in $(CURRENT_DIR)/$$i."; \
		$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
		'TOP=$(TOP)' clean) ;\
		done

realclean:
		rm -f *.log *.dvi *.aux *.ps *.toc *.blg *.bbl *.cb *~
		-rm -fr  $(INSTALL)/doc
		@for i in $(SUBDIRS) ;\
		do \
		(cd $$i; echo "making realclean in $(CURRENT_DIR)/$$i."; \
		$(MAKE) $(MFLAGS) "CURRENT_DIR=$(CURRENT_DIR)/$$i" \
		'TOP=$(TOP)' realclean) ;\
		done

depend:
