# $Id: Makefile,v 1.4 1998/12/29 05:21:41 gernot Exp $
include ../../src/Makefile.conf

MANDEST=$(INSTALL)/man/man2
CATDEST=$(INSTALL)/man/cat2
PSDEST=$(INSTALL)/man/ps2

SRC =	l4_fpage_unmap.man l4_id_nearest.man l4_mips_ipc.man \
	l4_mips_ipc_call.man l4_mips_ipc_chief_call.man \
	l4_mips_ipc_chief_receive.man l4_mips_ipc_chief_reply_and_wait.man \
	l4_mips_ipc_chief_send.man l4_mips_ipc_chief_wait.man \
	l4_mips_ipc_receive.man l4_mips_ipc_reply_and_wait.man \
	l4_mips_ipc_reply_deceiving_and_wait.man l4_mips_ipc_send.man \
	l4_mips_ipc_send_deceiving.man l4_mips_ipc_sleep.man \
	l4_mips_ipc_wait.man l4_myself.man l4_task_new.man \
	l4_thread_ex_regs.man l4_thread_schedule.man l4_thread_switch.man

all:

install:
	-mkdir -p $(PSDEST) $(MANDEST) $(CATDEST)
	@for i in $(SRC); do \
		j=`basename $$i .man`; \
		echo "Formatting $$i"; \
		cp $$i $(MANDEST)/$$j.2; \
		gnroff -man $$i > $(CATDEST)/$$j.2; \
		groff -man $$i | gzip --best > $(PSDEST)/$$j.ps.gz; \
	done
	$(MAKEWHATIS) $(MAKEWHATISFLAGS) $(MANDEST)/..

clean:
	rm -f *~ *.bak

realclean:	clean
	-rm -rf $(PSDEST) $(MANDEST) $(CATDEST)
