if BUILDVIEWER
  SUBDIRS = viewer
endif

bin_PROGRAMS = btgui

btgui_SOURCES = initwindow.cpp \
errordialog.cpp questiondialog.cpp prefdialog.cpp aboutdialog.cpp limitdialog.cpp sessiondialog.cpp \
sessionselection.cpp authdialog.cpp \
gtkmm_log.cpp guihandler.cpp traficmodel.cpp traficplot.cpp logtextview.cpp mainfiletreeview.cpp selectfiletreeview.cpp mainnotebook.cpp mainmodel.cpp filemodel.cpp fipixbox.cpp mainstatusbar.cpp mainmenubar.cpp maintreeview.cpp \
peermodel.cpp peertreeview.cpp \
detailsmodel.cpp detailstreeview.cpp \
mainwindow.cpp btgui.cpp

noinst_HEADERS = initwindow.h \
errordialog.h questiondialog.h prefdialog.h aboutdialog.h limitdialog.h sessiondialog.h \
sessionselection.h authdialog.h \
gtkmm_log.h guihandler.h traficmodel.h filemodel.h fipixbox.h traficplot.h logtextview.h mainfiletreeview.h selectfiletreeview.h mainnotebook.h mainmodel.h mainmenubar.h mainstatusbar.h maintreeview.h \
peermodel.h peertreeview.h \
detailsmodel.h detailstreeview.h \
mainwindow.h sig_button_menu.h \
gtkmm_ver.h usermessage.h

btgui_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                $(top_builddir)/bcore/client/libbtgclient.la \
                $(top_builddir)/external/ini/libini.la \
                $(top_builddir)/external/plotmm/libplotmm.la

btgui_LDFLAGS  = @LIBS@ @PCM_GTKMM_LIBS@ -l@BOOST_DATE_TIME_LIB@ -l@BOOST_IOSTREAMS_LIB@ -l@BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ -l@BOOST_PROGRAM_OPTIONS_LIB@ -l@BOOST_THREAD_LIB@
btgui_CXXFLAGS = @CXXFLAGS@ @PCM_GTKMM_CFLAGS@ @LIBGNUTLS_CFLAGS@ \
                 -DBTG_DEBUG=@BTG_DEBUG@ 

btgui_CFLAGS   = @CFLAGS@ @LIBGNUTLS_CFLAGS@ \
                 -DBTG_DEBUG=@BTG_DEBUG@ 

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/external
man1_MANS  = btgui.man
EXTRA_DIST = btgui.man
