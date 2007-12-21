if BUILDGUI
  BCORE_CLIENT = client
endif

if BUILDCLI
  BCORE_CLIENT = client
endif

# Build the current directory first, since the client stuff depends on
# commands etc.
SUBDIRS            = . $(BCORE_CLIENT)

pkglib_LTLIBRARIES    = libbtgcore.la

OS_SOURCES=os/sleep.cpp \
os/fileop.cpp \
os/id.cpp \
os/socket.cpp \
os/socket_exception.cpp \
os/socketgroup.cpp \
os/gnutlsif.cpp \
os/tcps.cpp \
os/stcps.cpp \
os/tcpc.cpp \
os/stcpc.cpp \
os/stdin.cpp \
os/exec.cpp \
os/timer.cpp \
os/gzipif.cpp \
os/gzip.cpp

TRANSPORT_SOURCES=transport/transport.cpp \
transport/tcpipbase.cpp \
transport/tcpip.cpp \
transport/stcpip.cpp \
transport/http.cpp \
transport/httprequest.cpp \
transport/shttp.cpp

COMMAND_SOURCES=command/ack.cpp \
command/command.cpp \
command/context.cpp \
command/context_abort.cpp \
command/context_clean.cpp \
command/context_clean_rsp.cpp \
command/context_create.cpp \
command/context_last.cpp \
command/context_start.cpp \
command/context_status.cpp \
command/context_fi.cpp \
command/context_status_rsp.cpp \
command/context_fi_rsp.cpp \
command/context_stop.cpp \
command/context_peers.cpp \
command/context_peers_rsp.cpp \
command/context_file.cpp \
command/context_tracker.cpp \
command/error.cpp \
command/list.cpp \
command/list_rsp.cpp \
command/limit_base.cpp \
command/context_limit.cpp \
command/context_li_status.cpp \
command/context_li_status_rsp.cpp \
command/session_attach.cpp \
command/session_detach.cpp \
command/session_list.cpp \
command/session_list_rsp.cpp \
command/session_quit.cpp \
command/session_error.cpp \
command/session_name.cpp \
command/session_rw.cpp \
command/setup.cpp \
command/kill.cpp \
command/limit.cpp \
command/uptime.cpp \
command/initconnection.cpp 

LOGGER_SOURCES=logger/logger.cpp \
logger/console_log.cpp \
logger/file_log.cpp \
logger/null_log.cpp \
logger/syslog_log.cpp

EXTERNALIZATION_SOURCES=externalization/externalization.cpp \
externalization/simple.cpp \
externalization/xmlrpc.cpp \
externalization/externalization_factory.cpp

AUTH_SOURCES=auth/hash.cpp

libbtgcore_la_SOURCES = project.cpp \
hru.cpp \
hrr.cpp \
hrt.cpp \
sigh.c \
util.cpp \
status.cpp \
trackerstatus.cpp \
peer.cpp \
file_info.cpp \
dbuf.cpp \
sbuf.cpp \
crashlog.cpp \
addrport.cpp \
configuration.cpp \
argif.cpp \
btg_assert.cpp \
$(OS_SOURCES) \
$(TRANSPORT_SOURCES) \
$(COMMAND_SOURCES) \
command_factory.cpp \
$(LOGGER_SOURCES) \
$(EXTERNALIZATION_SOURCES) \
$(AUTH_SOURCES)

libbtgcore_la_LIBADD = $(top_builddir)/external/xmlrpc-epi/src/libxmlrpc.la 

OS_HEADER=os/sleep.h \
os/fileop.h \
os/id.h \
os/socket.h \
os/socket_exception.h \
os/socketgroup.h \
os/gnutlsif.h \
os/gnutlsmacro.h \
os/tcps.h \
os/stcps.h \
os/tcpc.h \
os/stcpc.h \
os/fdcopy.h \
os/stdin.h \
os/exec.h \
os/timer.h \
os/gzipif.h \
os/gzip.h

TRANSPORT_HEADER=transport/transport.h \
transport/direction.h \
transport/tcpipbase.h \
transport/tcpip.h \
transport/stcpip.h \
transport/http.h \
transport/httprequest.h \
transport/shttp.h

COMMAND_HEADER=command/ack.h \
command/command.h \
command/context.h \
command/context_abort.h \
command/context_clean.h \
command/context_clean_rsp.h \
command/context_create.h \
command/context_last.h \
command/context_start.h \
command/context_status.h \
command/context_fi.h \
command/context_status_rsp.h \
command/context_fi_rsp.h \
command/context_stop.h \
command/context_peers.h \
command/context_peers_rsp.h \
command/context_file.h \
command/context_tracker.h \
command/error.h \
command/list.h \
command/list_rsp.h \
command/limit_base.h \
command/context_limit.h \
command/context_li_status.h \
command/context_li_status_rsp.h \
command/session_attach.h \
command/session_detach.h \
command/session_list.h \
command/session_list_rsp.h \
command/session_quit.h \
command/session_error.h \
command/session_name.h \
command/session_rw.h \
command/setup.h \
command/kill.h \
command/limit.h \
command/uptime.h \
command/initconnection.h

LOGGER_HEADER=logger/logif.h \
logger/logger.h \
logger/console_log.h \
logger/file_log.h \
logger/null_log.h \
logger/syslog_log.h

EXTERNALIZATION_HEADER=externalization/ext_macro.h \
externalization/externalization.h \
externalization/simple.h \
externalization/xmlrpc.h \
externalization/t_simple.h \
externalization/externalization_factory.h

AUTH_HEADER=auth/hash.h

noinst_HEADERS     = t_string.h type.h type_btg.h project.h \
transport/transport.h \
transport/tcpip.h \
transport/http.h \
hru.h hrr.h hrt.h \
sigh.h \
logmacro.h util.h status.h trackerstatus.h peer.h file_info.h dbuf.h sbuf.h \
crashlog.h \
serializable.h printable.h addrport.h \
helpermacro.h configuration.h \
argif.h \
btg_assert.h \
verbose.h \
copts.h \
$(OS_HEADER) \
$(TRANSPORT_HEADER) \
$(COMMAND_HEADER) \
command_factory.h \
$(LOGGER_HEADER) \
$(EXTERNALIZATION_HEADER) \
$(AUTH_HEADER)

AM_CFLAGS           = @CFLAGS@   @DEBUG_CFLAGS@ \
                    -DBTG_DEBUG=@BTG_DEBUG@ \
                    -DBTG_VERSION="@VERSION@" \
                    -DBTG_BUILD="@BTG_BUILD@" \
                    -DBTG_SVNREV="@BTG_SVNREV@"

AM_CXXFLAGS         = @CXXFLAGS@ @DEBUG_CXXFLAGS@ \
                    -DBTG_DEBUG=@BTG_DEBUG@ \
                    -DBTG_VERSION="@VERSION@" \
                    -DBTG_BUILD="@BTG_BUILD@" \
                    -DBTG_SVNREV="@BTG_SVNREV@"

INCLUDES            = -I$(top_srcdir) 
