WWW_UI_DEST  = $(DESTDIR)/$(datadir)/wwwbtg/htdocs/bcore/command
WWW_UI_FILES = \
ack.php \
command.php \
context_abort.php \
context_clean.php \
context_clean_rsp.php \
context_create.php \
context_fi.php \
context_fi_rsp.php \
context_last.php \
context_limit.php \
context_li_status.php \
context_li_status_rsp.php \
context_peers.php \
context_peers_rsp.php \
context.php \
context_start.php \
context_status.php \
context_status_rsp.php \
context_stop.php \
context_tracker.php \
error.php \
_gencommands.php \
initconnection.php \
initsession.php \
kill.php \
limit_base.php \
limit.php \
list.php \
list_rsp.php \
session_attach.php \
session_detach.php \
session_error.php \
session_list.php \
session_list_rsp.php \
session_name.php \
session_quit.php \
setup.php \
uptime.php

EXTRA_DIST = $(WWW_UI_FILES)

if BUILDWWW

install-data-local:
	$(mkinstalldirs) $(WWW_UI_DEST)
	for file in $(WWW_UI_FILES) ; do \
		if [ -e $$file ] ; then \
			$(INSTALL_DATA) $$file $(WWW_UI_DEST) ; \
		else \
			$(INSTALL_DATA) $(srcdir)/$$file $(WWW_UI_DEST) ; \
		fi ; \
	done

endif

