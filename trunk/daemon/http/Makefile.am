if BUILDURL
  noinst_LTLIBRARIES   = libhttp.la
  noinst_HEADERS = httpif.h curlif.h httpproc.h httpmgr.h
  libhttp_la_SOURCES = curlif.cpp httpif.cpp httpproc.cpp httpmgr.cpp
  libhttp_la_CFLAGS  = @CFLAGS@ @DEBUG_CFLAGS@ \
  -DBTG_DEBUG=@BTG_DEBUG@ -DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
  -DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
  -DBTG_SVNREV="@BTG_SVNREV@" @LIBCURL_CPPFLAGS@

  libhttp_la_CXXFLAGS  = @CXXFLAGS@ @DEBUG_CXXFLAGS@ \
  -DBTG_DEBUG=@BTG_DEBUG@ -DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
  -DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
  -DBTG_SVNREV="@BTG_SVNREV@" @LIBCURL_CPPFLAGS@

  INCLUDES = @LIBTORRENT_CFLAGS@ -I$(top_srcdir)
else
  EXTRA_DIST = httpif.h curlif.h httpproc.h httpmgr.h \
               curlif.cpp httpif.cpp httpproc.cpp httpmgr.cpp
endif