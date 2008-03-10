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
                  numinput.cpp \
                  mainwindow.cpp \
                  ui.cpp ui_menu.cpp ui_handler.cpp \
                  handler.cpp \
                  btgncli.cpp

noinst_HEADERS = nconfig.h \
                 keys.h colors.h \
                 window.h \
                 basemenu.h \
                 initwindow.h \
                 detailwindow.h statuswindow.h topwindow.h snwindow.h \
                 helpwindow.h filelist.h limitwindow.h peerlist.h \
                 fileview.h \
                 sessionselect.h \
                 fileselect.h \
                 numinput.cpp \
                 mainwindow.h \
                 ui.h \
                 handler.h

btgncli_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                  $(top_builddir)/bcore/client/libbtgclient.la \
                  $(top_builddir)/external/ini/libini.la \
                  $(top_builddir)/bcore/libbtgcore.la

btgncli_LDFLAGS = @LIBS@ @CURSES_LIB@ -l@BOOST_DATE_TIME_LIB@ -l@BOOST_IOSTREAMS_LIB@ -l@BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ -l@BOOST_PROGRAM_OPTIONS_LIB@ -l@BOOST_THREAD_LIB@ 

btgncli_CFLAGS   = @CFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_SVNREV="@BTG_SVNREV@"

btgncli_CXXFLAGS = @CXXFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_SVNREV="@BTG_SVNREV@"

INCLUDES = -I$(top_srcdir)

man1_MANS  = btgncli.man
EXTRA_DIST = btgncli.man
