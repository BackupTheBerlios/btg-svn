pkglib_LTLIBRARIES = libbtgclient.la

libbtgclient_la_SOURCES = carg.cpp \
configuration.cpp lastfiles.cpp\
statemachine_cb.cpp \
statemachine.cpp \
clienthandler.cpp \
clientcallback.cpp \
helper_argif.cpp \
helper.cpp \
handlerthr.cpp \
clientdynconfig.cpp

noinst_HEADERS = carg.h \
configuration.h lastfiles.h \
statemachine.h \
clienthandler.h \
clientcallback.h \
helper_argif.h \
helper.h \
handlerthr.h \
clientdynconfig.h

libbtgclient_la_CFLAGS   = @DEBUG_CFLAGS@ -DBTG_DEBUG=@BTG_DEBUG@
libbtgclient_la_CXXFLAGS = @DEBUG_CXXFLAGS@ -DBTG_DEBUG=@BTG_DEBUG@
libbtgclient_la_LDFLAGS  = -l@BOOST_PROGRAM_OPTIONS_LIB@

INCLUDES             = -I$(top_srcdir)
