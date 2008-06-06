#libini_la_CFLAGS = $(CFLAGS)
#libinit_la_LIBADD = 

noinst_LTLIBRARIES = libini.la
libini_la_SOURCES  = cdatafile.cpp
noinst_HEADERS     = cdatafile.h

AM_CFLAGS           = @CFLAGS@   @DEBUG_CFLAGS@ \
                    -DBTG_DEBUG=@BTG_DEBUG@ \
                    -DBTG_VERSION="@VERSION@" \
                    -DBTG_BUILD="@BTG_BUILD@"

AM_CXXFLAGS         = @CXXFLAGS@ @DEBUG_CXXFLAGS@ \
                    -DBTG_DEBUG=@BTG_DEBUG@ \
                    -DBTG_VERSION="@VERSION@" \
                    -DBTG_BUILD="@BTG_BUILD@"
