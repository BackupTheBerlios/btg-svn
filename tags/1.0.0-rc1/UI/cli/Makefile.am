SUBDIRS = ncurses

if BUILDCLI
bin_PROGRAMS = btgcli

btgcli_SOURCES = clicmd.cpp cli_create.cpp cli.cpp screen.cpp screen_log.cpp btgcli.cpp
noinst_HEADERS = runstate.h clicmd.h cli.h screen.h screen_log.h
btgcli_LDADD   = $(top_builddir)/UI/cli/ncurses/libcommoncli.la \
                 $(top_builddir)/bcore/libbtgcore.la \
                 $(top_builddir)/bcore/client/libbtgclient.la \
                 $(top_builddir)/external/ini/libini.la \
                 $(top_builddir)/bcore/libbtgcore.la

btgcli_LDFLAGS = @LIBS@ @BOOST_SYSTEM_LIB@ @BOOST_DATE_TIME_LIB@ @BOOST_IOSTREAMS_LIB@ @BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ @BOOST_PROGRAM_OPTIONS_LIB@ @BOOST_THREAD_LIB@
btgcli_CFLAGS   = @CFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@" \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

btgcli_CXXFLAGS = @CXXFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@" \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

INCLUDES = -I$(top_srcdir)

man1_MANS  = btgcli.man
endif # BUILDCLI

EXTRA_DIST = btgcli.man
