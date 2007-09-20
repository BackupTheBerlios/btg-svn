WWW_UI_DEST  = $(DESTDIR)/$(datadir)/wwwbtg/htdocs/images
WWW_UI_FILES = bottom.gif btn1.gif \
btn2.gif middle.gif progress_download.gif \
progress_finish.gif progress_seed.gif \
rowhover.gif top.gif

EXTRA_DIST = $(WWW_UI_FILES)

if BUILDWWW

install-data-local:
	$(mkinstalldirs) $(WWW_UI_DEST)
	$(INSTALL_DATA) $(WWW_UI_FILES) $(WWW_UI_DEST)

endif

