WWW_UI_DEST  = $(DESTDIR)/$(datadir)/wwwbtg/htdocs/images
WWW_UI_FILES = bottom.gif btn1.gif \
btn2.gif middle.gif progress_download.gif \
progress_finish.gif progress_seed.gif \
rowhover.gif top.gif

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

