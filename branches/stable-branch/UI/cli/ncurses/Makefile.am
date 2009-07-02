if BUILDCLI

noinst_LTLIBRARIES   = libcommoncli.la

noinst_HEADERS = cli_helper.h \
                 se.h dm.h

libcommoncli_la_SOURCES = cli_helper.cpp \
                          se.cpp dm.cpp

libcommoncli_la_CXXFLAGS = @CXXFLAGS@ @LIBGNUTLS_CFLAGS@ @ZLIB_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@" \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

#
#
#

bin_PROGRAMS = btgncli

btgncli_SOURCES = nconfig.cpp \
                  keys.cpp colors.cpp \
                  window.cpp \
                  basemenu.cpp \
                  initwindow.cpp \
                  detailwindow.cpp statuswindow.cpp topwindow.cpp snwindow.cpp statswindow.cpp \
                  helpwindow.cpp filelist.cpp limitwindow.cpp peerlist.cpp \
                  fileview.cpp \
                  sessionselect.cpp \
                  fileselect.cpp \
                  numinput.cpp textinput.cpp \
                  mainwindow.cpp \
                  progress.cpp \
                  ui.cpp ui_create.cpp ui_menu.cpp ui_handler.cpp \
                  handler.cpp \
                  btgncli.cpp

noinst_HEADERS += nconfig.h \
                 keys.h colors.h \
                 window.h \
                 basemenu.h \
                 initwindow.h \
                 detailwindow.h statuswindow.h topwindow.h snwindow.h statswindow.h \
                 helpwindow.h filelist.h limitwindow.h peerlist.h \
                 fileview.h \
                 sessionselect.h \
                 fileselect.h \
                 numinput.h textinput.h \
                 mainwindow.h \
                 progress.h \
                 ui.h \
                 handler.h

btgncli_LDADD   = libcommoncli.la \
                  $(top_builddir)/bcore/libbtgcore.la \
                  $(top_builddir)/bcore/client/libbtgclient.la \
                  $(top_builddir)/external/ini/libini.la \
                  $(top_builddir)/bcore/libbtgcore.la

btgncli_LDFLAGS = @LIBS@ @CURSES_LIB@ @BOOST_SYSTEM_LIB@ @BOOST_DATE_TIME_LIB@ @ZLIB_LDFLAGS@ @BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ @BOOST_PROGRAM_OPTIONS_LIB@ @BOOST_THREAD_LIB@ 

btgncli_CFLAGS   = @CFLAGS@ @LIBGNUTLS_CFLAGS@ @ZLIB_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@" \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

btgncli_CXXFLAGS = @CXXFLAGS@ @LIBGNUTLS_CFLAGS@ @ZLIB_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@" \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

INCLUDES = -I$(top_srcdir)

man1_MANS  = btgncli.man

endif # BUILDCLI

EXTRA_DIST = btgncli.man
