if BUILDCLI

noinst_LTLIBRARIES   = libcommoncli.la

noinst_HEADERS = cli_helper.h

libcommoncli_la_SOURCES = cli_helper.cpp

libcommoncli_la_CXXFLAGS = @CXXFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@"

#
#
#

bin_PROGRAMS = btgncli

btgncli_SOURCES = nconfig.cpp \
                  keys.cpp colors.cpp \
                  window.cpp \
                  basemenu.cpp \
                  initwindow.cpp \
                  detailwindow.cpp statuswindow.cpp topwindow.cpp snwindow.cpp \
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
                 detailwindow.h statuswindow.h topwindow.h snwindow.h \
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

btgncli_LDFLAGS = @LIBS@ @CURSES_LIB@ @BOOST_SYSTEM_LIB@ @BOOST_DATE_TIME_LIB@ @BOOST_IOSTREAMS_LIB@ @BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ @BOOST_PROGRAM_OPTIONS_LIB@ @BOOST_THREAD_LIB@ 

btgncli_CFLAGS   = @CFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@"

btgncli_CXXFLAGS = @CXXFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@"

INCLUDES = -I$(top_srcdir)

man1_MANS  = btgncli.man
EXTRA_DIST = btgncli.man

endif # BUILDCLI
