if BUILDUPNP
  EXTRA_DIR = upnp
  UPNP_LDADD = $(top_builddir)/daemon/upnp/libupnp.la 
endif

if BUILDURL
  URL_HEADER=urlmgr.h
  URL_SOURCE=urlmgr.cpp
  URL_DEFINES=-DBTG_OPTION_URL=@BTG_OPTION_URL@ @LIBCURL_CPPFLAGS@
  URL_LIBS=@LIBCURL@
else
  URL_DEFINES=-DBTG_OPTION_URL=@BTG_OPTION_URL@
endif

SUBDIRS = http resource ipfilter auth $(EXTRA_DIR)

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
                 opid.h \
                 dconfig.h daemon.h daemondata.h \
                 daemonhandler.h \
                 filemgr.h $(URL_HEADER) \
                 btgdaemon.h \
                 progress.h \
                 lt_version.h \
                 session_list.h \
                 portmgr.h limitval.h limitmgr.h \
                 modulelog.h \
                 connectionextrastate.h

libdaemon_la_SOURCES = dconfig.cpp darg.cpp \
                       opid.cpp \
                       filetrackdata.cpp filetrack.cpp connectionhandler.cpp \
                       session_list.cpp portmgr.cpp \
                       limitval.cpp limitmgr.cpp daemondata.cpp \
                       daemonhandler_url.cpp daemonhandler_file.cpp daemonhandler.cpp progress.cpp \
                       filemgr.cpp $(URL_SOURCE) \
                       context_alert.cpp context_file.cpp context_session.cpp context.cpp \
                       eventhandler.cpp connectionextrastate.cpp $(SSSOURCES) $(CALLBACKSOURCES)

libdaemon_la_CFLAGS   = @CFLAGS@ @DEBUG_CFLAGS@ -DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
-DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
-DBTG_OPTION_UPNP=@BTG_OPTION_UPNP@ \
-DBTG_OPTION_USELTUPNP=@BTG_OPTION_USELTUPNP@ \
-DBTG_OPTION_USECYBERLINK=@BTG_OPTION_USECYBERLINK@ @CYBERLINK_CFLAGS@ \
-DBTG_REV="@BTG_REV@" $(URL_DEFINES) \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

libdaemon_la_CXXFLAGS = @CXXFLAGS@ @DEBUG_CXXFLAGS@ -DBTG_DEBUG=@BTG_DEBUG@ \
-DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
-DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
-DBTG_OPTION_UPNP=@BTG_OPTION_UPNP@ \
-DBTG_OPTION_USELTUPNP=@BTG_OPTION_USELTUPNP@ \
-DBTG_OPTION_USECYBERLINK=@BTG_OPTION_USECYBERLINK@ @CYBERLINK_CFLAGS@ \
-DBTG_REV="@BTG_REV@" $(URL_DEFINES) \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

bin_PROGRAMS = btgdaemon 

btgdaemon_SOURCES = btgdaemon.cpp daemon.c

if BUILDCYBERLINK
  CYBERLINKLDFLAGS = @CYBERLINK_LIBS@
endif

if BUILDURL
  HTTP_LDADD = $(top_builddir)/daemon/http/libhttp.la 
endif

btgdaemon_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                    libdaemon.la \
                    $(top_builddir)/daemon/resource/libresource.la \
                    $(top_builddir)/daemon/ipfilter/libipfilter.la \
                    $(top_builddir)/daemon/auth/libauth.la \
                    $(top_builddir)/external/ini/libini.la \
                    $(top_builddir)/bcore/libbtgcore.la \
                    $(UPNP_LDADD) $(HTTP_LDADD)

btgdaemon_LDFLAGS = @LIBS@ @LIBTORRENT_LIBS@ @BOOST_SYSTEM_LIB@ @BOOST_IOSTREAMS_LIB@ \
                    @LIBGNUTLS_LIBS@ @BOOST_THREAD_LIB@ @BOOST_FILESYSTEM_LIB@ \
                    @BOOST_PROGRAM_OPTIONS_LIB@ @XMLRPC_LDFLAGS@ $(URL_LIBS) $(CYBERLINKLDFLAGS)

btgdaemon_CFLAGS   = @CFLAGS@   @DEBUG_CFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ -DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
-DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
-DBTG_OPTION_UPNP=@BTG_OPTION_UPNP@ \
-DBTG_OPTION_USELTUPNP=@BTG_OPTION_USELTUPNP@ \
-DBTG_OPTION_USECYBERLINK=@BTG_OPTION_USECYBERLINK@ @CYBERLINK_CFLAGS@ $(URL_DEFINES) \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

btgdaemon_CXXFLAGS = @CXXFLAGS@ @DEBUG_CXXFLAGS@ @LIBGNUTLS_CFLAGS@ \
-DBTG_DEBUG=@BTG_DEBUG@ -DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
-DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
-DBTG_OPTION_UPNP=@BTG_OPTION_UPNP@ \
-DBTG_OPTION_USELTUPNP=@BTG_OPTION_USELTUPNP@ \
-DBTG_OPTION_USECYBERLINK=@BTG_OPTION_USECYBERLINK@ @CYBERLINK_CFLAGS@ $(URL_DEFINES) \
-DBTG_INTERNAL_XMLRPC="@BTG_INTERNAL_XMLRPC@"

INCLUDES = @LIBTORRENT_CFLAGS@ -I$(top_srcdir)

$(top_srcdir)/datestamp:
	$(touch) darg.cpp
	$(touch) darg.h

man1_MANS          = btgdaemon.man
man5_MANS          = btgcallback.man

if !BUILDURL
  EXTRA_DIST_HTTP = urlmgr.h http/curlif.h http/httpmgr.h http/httpif.h http/httpproc.h \
                    urlmgr.cpp http/curlif.cpp http/httpmgr.cpp http/httpif.cpp \
                    http/httpproc.cpp
endif

# Since the session saver is optional, and not always compiled, make dist needs this.
EXTRA_DIST         = sessionsaver.cpp sessionsaver.h \
                     btgdaemon.man btgcallback.man $(EXTRA_DIST_HTTP)

