bin_PROGRAMS = btgvs

btgvs_SOURCES = arg.cpp handler.cpp ui.cpp ui_style.cpp btgvs.cpp

noinst_HEADERS = agar-if.h arg.h handler.h ui.h btgvs.h

btgvs_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                $(top_builddir)/bcore/client/libbtgclient.la \
                $(top_builddir)/external/ini/libini.la \
                $(top_builddir)/bcore/libbtgcore.la

btgvs_LDFLAGS  = @LIBS@ -l@BOOST_DATE_TIME_LIB@ -l@BOOST_IOSTREAMS_LIB@ -l@BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ -l@BOOST_PROGRAM_OPTIONS_LIB@ -l@BOOST_THREAD_LIB@ @AGAR_LIBS@

btgvs_CXXFLAGS = @CXXFLAGS@ @LIBGNUTLS_CFLAGS@ @AGAR_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@"

btgvs_CFLAGS   = @CFLAGS@ @LIBGNUTLS_CFLAGS@ @AGAR_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@"

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/external
man1_MANS  = btgvs.man
EXTRA_DIST = btgvs.man
