SUBDIRS = commands site

all:

.PHONY : doxygen

doxygen:
	@echo "Generating docs with doxygen.."
	doxygen

clean:
	@rm -fr doxygen

man5_MANS  = btg_config.man btg.man
EXTRA_DIST = btg_config.man btg.man Doxyfile
