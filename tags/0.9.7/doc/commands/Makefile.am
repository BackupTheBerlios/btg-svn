if BUILDCOMMANDLIST

noinst_PROGRAMS = gen_cmd_docs

endif

gen_cmd_docs_SOURCES = ext_printer.cpp gen_cmd_docs.cpp $(top_srcdir)/bcore/file_info.cpp $(top_srcdir)/bcore/status.cpp $(top_srcdir)/bcore/peer.cpp $(top_srcdir)/bcore/sbuf.cpp $(top_srcdir)/bcore/addrport.cpp

noinst_HEADERS       = ext_printer.h

gen_cmd_docs_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                       $(top_builddir)/external/ini/libini.la

gen_cmd_docs_LDFLAGS = @LIBS@ @LIBTORRENT_LIBS@ -l@BOOST_IOSTREAMS_LIB@ -l@BOOST_PROGRAM_OPTIONS_LIB@ @LIBGNUTLS_LIBS@ 

gen_cmd_docs_CFLAGS   = @CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_SVNREV="@BTG_SVNREV@"

gen_cmd_docs_CXXFLAGS = @CXXFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_SVNREV="@BTG_SVNREV@"

INCLUDES = -I$(top_srcdir)

commanddoc: commands_prepend.rst gen_cmd_docs
	@echo "Generating list of commands (.rst file)"
	@./gen_cmd_docs > commands.rst
	@cat commands_prepend.rst > commands_temp.rst
	@cat commands.rst >> commands_temp.rst
	@echo "Generating list of commands (html file)"
	@rst2html --strict commands_temp.rst > commands.html
	@rm -f commands_temp.rst
