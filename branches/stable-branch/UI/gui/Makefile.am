if BUILDVIEWER
  SUBDIRS = viewer
endif

if BUILDGUI

bin_PROGRAMS = btgui

btgui_SOURCES = initwindow.cpp progressdialog.cpp urldialog.cpp \
errordialog.cpp questiondialog.cpp prefdialog.cpp aboutdialog.cpp limitdialog.cpp sessiondialog.cpp \
sessionselection.cpp authdialog.cpp sndialog.cpp \
gtkmm_log.cpp guihelper.cpp guihandler.cpp traficmodel.cpp traficplot.cpp logtextview.cpp mainfiletreeview.cpp selectfiletreeview.cpp mainnotebook.cpp mainmodel.cpp filemodel.cpp fipixbox.cpp mainstatusbar.cpp mainmenubar.cpp maintreeview.cpp \
peermodel.cpp peertreeview.cpp \
detailsmodel.cpp detailstreeview.cpp \
mainwindow_create.cpp mainwindow.cpp btgui.cpp guiutils.cpp

noinst_HEADERS = initwindow.h progressdialog.h urldialog.h \
errordialog.h questiondialog.h prefdialog.h aboutdialog.h aboutimage.h \
limitdialog.h sessiondialog.h \
sessionselection.h authdialog.h sndialog.h \
gtkmm_log.h guihelper.h guihandler.h traficmodel.h filemodel.h fipixbox.h \
traficplot.h logtextview.h mainfiletreeview.h selectfiletreeview.h mainnotebook.h \
mainmodel.h mainmenubar.h mainstatusbar.h maintreeview.h \
peermodel.h peertreeview.h \
detailsmodel.h detailstreeview.h \
mainwindow.h sig_button_menu.h \
usermessage.h guiutils.h

btgui_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                $(top_builddir)/bcore/client/libbtgclient.la \
                $(top_builddir)/external/ini/libini.la \
                $(top_builddir)/bcore/libbtgcore.la

if BUILDINTERNALPLOTMM
btgui_LDADD  += $(top_builddir)/external/plotmm/libplotmm.la
endif

btgui_LDFLAGS  = @LIBS@ @GTKMM_LIBS@ @BOOST_SYSTEM_LIB@ @BOOST_DATE_TIME_LIB@ @ZLIB_LDFLAGS@ @BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ @BOOST_PROGRAM_OPTIONS_LIB@ @BOOST_THREAD_LIB@ @PLOTMM_LDFLAGS@

btgui_CXXFLAGS = @CXXFLAGS@ @XMLRPC_CFLAGS@ @GTKMM_CFLAGS@ @LIBGNUTLS_CFLAGS@ @PLOTMM_CXXFLAGS@ @ZLIB_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_REV="@BTG_REV@" \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

btgui_CFLAGS   = @CFLAGS@ @XMLRPC_CFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ @ZLIB_CFLAGS@ \
-DBTG_REV="@BTG_REV@" \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/external
man1_MANS  = btgui.man

endif # build gui

EXTRA_DIST = btgui.man
