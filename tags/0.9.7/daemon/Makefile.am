if BUILDUPNP
  EXTRA_DIR = upnp
endif

SUBDIRS = resource ipfilter auth $(EXTRA_DIR)

if BUILDSESSIONSAVING
  SSSOURCES = sessionsaver.cpp 
  SSHEADERS = sessionsaver.h
endif

if BUILDEVENTCALLBACK
  CALLBACKSOURCES = callbackmgr.cpp auth/auth.cpp
  CALLBACKHEADERS = callbackmgr.h
endif

noinst_LTLIBRARIES   = libdaemon.la

noinst_HEADERS = $(CALLBACKHEADERS) $(SSHEADERS) filetrackdata.h filetrack.h connectionhandler.h context.h \
                 eventhandler.h darg.h \
                 dconfig.h daemon.h daemondata.h \
                 daemonhandler.h \
                 btgdaemon.h \
                 progress.h \
                 lt_version.h \
                 session_list.h \
                 portmgr.h limitval.h limitmgr.h \
                 modulelog.h \
                 connectionextrastate.h

libdaemon_la_SOURCES = dconfig.cpp darg.cpp \
                       filetrackdata.cpp filetrack.cpp connectionhandler.cpp \
                       session_list.cpp portmgr.cpp \
                       limitval.cpp limitmgr.cpp daemondata.cpp \
                       daemonhandler.cpp progress.cpp \
                       context_alert.cpp context_file.cpp context_session.cpp context.cpp \
                       eventhandler.cpp connectionextrastate.cpp $(SSSOURCES) $(CALLBACKSOURCES)

libdaemon_la_CFLAGS   = @CFLAGS@ @DEBUG_CFLAGS@ -DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
-DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
-DBTG_OPTION_UPNP=@BTG_OPTION_UPNP@ \
-DBTG_OPTION_USECYBERLINK=@BTG_OPTION_USECYBERLINK@ @CYBERLINK_CFLAGS@ \
-DBTG_SVNREV="@BTG_SVNREV@"

libdaemon_la_CXXFLAGS = @CXXFLAGS@ @DEBUG_CXXFLAGS@ -DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
-DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
-DBTG_OPTION_UPNP=@BTG_OPTION_UPNP@ \
-DBTG_OPTION_USECYBERLINK=@BTG_OPTION_USECYBERLINK@ @CYBERLINK_CFLAGS@ \
-DBTG_SVNREV="@BTG_SVNREV@"

bin_PROGRAMS = btgdaemon 

btgdaemon_SOURCES = btgdaemon.cpp daemon.c

if BUILDCYBERLINK
  EXTRA_LDADD=$(top_builddir)/daemon/upnp/libupnp.la @CYBERLINK_LDFLAGS@
endif

btgdaemon_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                    libdaemon.la \
                    $(top_builddir)/daemon/resource/libresource.la \
                    $(top_builddir)/daemon/ipfilter/libipfilter.la \
                    $(top_builddir)/daemon/auth/libauth.la $(EXTRA_LDADD) \
                    $(top_builddir)/external/ini/libini.la \
                    $(top_builddir)/bcore/libbtgcore.la

#                    $(top_builddir)/bcore/libcore.la \
#                    $(top_builddir)/bcore/externalization/libextern.la \
#                    $(top_builddir)/bcore/transport/libtransport.la \
#                    $(top_builddir)/bcore/os/libos.la \
#                    $(top_builddir)/bcore/command/libcommand.la \
#                    $(top_builddir)/bcore/logger/liblogger.la \
#                    $(top_builddir)/external/ini/libini.la \
#                    $(top_builddir)/bcore/auth/libbhash.la

btgdaemon_LDFLAGS  = @LIBS@ @LIBTORRENT_LIBS@ -l@BOOST_IOSTREAMS_LIB@ @LIBGNUTLS_LIBS@ \
-l@BOOST_PROGRAM_OPTIONS_LIB@

btgdaemon_CFLAGS   = @CFLAGS@   @DEBUG_CFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ -DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
-DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
-DBTG_OPTION_UPNP=@BTG_OPTION_UPNP@ -DBTG_OPTION_USECYBERLINK=@BTG_OPTION_USECYBERLINK@ @CYBERLINK_CFLAGS@

btgdaemon_CXXFLAGS = @CXXFLAGS@ @DEBUG_CXXFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ -DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
-DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
-DBTG_OPTION_UPNP=@BTG_OPTION_UPNP@ -DBTG_OPTION_USECYBERLINK=@BTG_OPTION_USECYBERLINK@ @CYBERLINK_CPPFLAGS@

INCLUDES = @LIBTORRENT_CFLAGS@ -I$(top_srcdir)

$(top_srcdir)/datestamp:
	$(touch) darg.cpp
	$(touch) darg.h

man1_MANS          = btgdaemon.man
man5_MANS          = btgcallback.man

# Since the session saver is optional, and not always compiled, make dist needs this.
EXTRA_DIST         = sessionsaver.cpp sessionsaver.h btgdaemon.man btgcallback.man
