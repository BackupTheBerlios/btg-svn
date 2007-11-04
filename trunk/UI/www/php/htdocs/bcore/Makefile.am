SUBDIRS = command

WWW_UI_DEST  = $(DESTDIR)/$(datadir)/wwwbtg/htdocs/bcore
WWW_UI_FILES = addrport.php btgexception.php \
command_factory.php file_info.php hash.php peer.php \
sbuf.php serializable.php status.php xmlrpc.php trackerstatus.php

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
