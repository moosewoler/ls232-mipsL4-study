include ../src/Makefile.conf

# This is only to install the proper example directory.

all:

install:
	-mkdir -p $(INSTALL)/example
	/bin/cp -p README.templ $(INSTALL)/example/README
	/bin/cp -p main.c $(INSTALL)/example
	sed -e 's,@BOOT@,$(BOOT),' -e 's,@XDEVBIN@,$(XDEVBIN),' \
		-e 's,@INSTALL@,$(INSTALL),' \
		Makefile.templ > $(INSTALL)/example/Makefile

clean:

realclean:
	-rm -rf $(INSTALL)/example

depend:
