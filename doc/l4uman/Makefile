# $Id: Makefile,v 1.3 1999/08/11 02:33:34 gernot Exp $
include ../../src/Makefile.conf

UMAN=$(INSTALL)/doc/l4uman.ps.gz

ROOT=l4uman
MANDIR=man
SRC=intro.tex l4ipc.tex l4nonipc.tex l4uman.tex l4use.tex

all:		$(ROOT).ps

install:	$(ROOT).ps
		-mkdir -p $(INSTALL)/doc
		gzip --best <$(ROOT).ps >$(UMAN)

$(ROOT).ps:	$(ROOT).dvi
		TEXINPUTS=sty:$$TEXINPUTS: dvips -o $(ROOT).ps $(ROOT)

$(ROOT).dvi:	$(SRC)
		TEXINPUTS=sty:$$TEXINPUTS: latex $(ROOT)
		TEXINPUTS=sty:$$TEXINPUTS: latex $(ROOT)

clean:
		rm -f *.log *.dvi *.aux *.ps *.toc *.blg *.cb *~

realclean:	clean
		-rm -fr  $(INSTALL)/doc
