SUBDIRS = commands site

all:

.PHONY : doxygen

doxygen:
	@echo "Generating docs with doxygen.."
	rm -Rf $(srcdir)/doxygen
	cd $(srcdir) && doxygen && echo Documentation ready in $(srcdir)/doxygen

clean:
	@rm -fr doxygen

man5_MANS  = btg_config.man btg.man
EXTRA_DIST = btg_config.man btg.man Doxyfile
