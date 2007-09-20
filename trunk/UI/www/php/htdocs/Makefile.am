SUBDIRS = bcore images

WWW_UI_DEST  = $(DESTDIR)/$(datadir)/wwwbtg/htdocs
WWW_UI_FILES = ajaxjs.php aphpstr.php btg.css \
btg.js btg.php config.php \
csshover.js index.php phpajax.php upload.php version.php

EXTRA_DIST = $(WWW_UI_FILES)

if BUILDWWW

install-data-local:
	$(mkinstalldirs) $(WWW_UI_DEST)
	$(INSTALL_DATA) $(WWW_UI_FILES) $(WWW_UI_DEST)

endif
