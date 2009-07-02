bin_PROGRAMS = btgvs

btgvs_SOURCES = arg.cpp handler.cpp ui.cpp ui_style.cpp btgvs.cpp

noinst_HEADERS = agar-if.h arg.h handler.h ui.h

btgvs_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                  $(top_builddir)/bcore/client/libbtgclient.la \
                  $(top_builddir)/external/ini/libini.la \
                  $(top_builddir)/bcore/libbtgcore.la

btgvs_LDFLAGS = @LIBS@ @AGAR_LIBS@ @BOOST_SYSTEM_LIB@ @BOOST_DATE_TIME_LIB@ @ZLIB_LDFLAGS@ @BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ @BOOST_PROGRAM_OPTIONS_LIB@ @BOOST_THREAD_LIB@ @LIRC_CLIENT_LIB@

btgvs_CXXFLAGS = @CXXFLAGS@ @XMLRPC_CFLAGS@ @LIBGNUTLS_CFLAGS@ @AGAR_CFLAGS@ @ZLIB_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@" \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

btgvs_CFLAGS   = @CFLAGS@ @XMLRPC_CFLAGS@ @LIBGNUTLS_CFLAGS@ @AGAR_CFLAGS@ @ZLIB_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@" \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/external
man1_MANS  = btgvs.man
EXTRA_DIST = btgvs.man
