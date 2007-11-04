SUBDIRS = bcore images

WWW_UI_DEST  = $(DESTDIR)/$(datadir)/wwwbtg/htdocs
WWW_UI_FILES = ajaxjs.php aphpstr.php btg.css \
btg.js btg.php config.php \
csshover.js index.php phpajax.php upload.php version.php

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
