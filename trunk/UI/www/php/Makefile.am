SUBDIRS = htdocs

WWW_UI_DEST  = $(DESTDIR)/$(datadir)/wwwbtg
WWW_UI_FILES = README-TEST test.php
EXTRA_DIST   = $(WWW_UI_FILES)

if BUILDWWW

install-data-local:
	$(mkinstalldirs) $(WWW_UI_DEST)
	$(INSTALL_DATA) $(WWW_UI_FILES) $(WWW_UI_DEST)

endif
