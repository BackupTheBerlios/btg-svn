SUBDIRS = ncurses

bin_PROGRAMS = btgcli

btgcli_SOURCES = cli.cpp nscreen.cpp nscreen_log.cpp btgcli.cpp
noinst_HEADERS = runstate.h cli.h nscreen.h nscreen_log.h
btgcli_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                 $(top_builddir)/bcore/client/libbtgclient.la \
                 $(top_builddir)/external/ini/libini.la

btgcli_LDFLAGS = @LIBS@ @CURSES_LIB@ -l@BOOST_DATE_TIME_LIB@ -l@BOOST_IOSTREAMS_LIB@ -l@BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ -l@BOOST_PROGRAM_OPTIONS_LIB@ -l@BOOST_THREAD_LIB@
btgcli_CFLAGS   = @CFLAGS@ @LIBGNUTLS_CFLAGS@ \
                  -DBTG_DEBUG=@BTG_DEBUG@
btgcli_CXXFLAGS = @CXXFLAGS@ @LIBGNUTLS_CFLAGS@ \
                  -DBTG_DEBUG=@BTG_DEBUG@

INCLUDES = -I$(top_srcdir)

man1_MANS  = btgcli.man
EXTRA_DIST = btgcli.man
