INCLUDES= -I$(top_srcdir)/external/xmlrpc-epi/expat/xmltok -I$(top_srcdir)/external/xmlrpc-epi/expat/xmlparse
libxmlrpc_la_CFLAGS = $(CFLAGS)

noinst_LTLIBRARIES = libxmlrpc.la

libxmlrpc_la_LIBADD = $(top_builddir)/external/xmlrpc-epi/expat/xmltok/libexpat_tok.la $(top_builddir)/external/xmlrpc-epi/expat/xmlparse/libexpat_parse.la

libxmlrpc_la_SOURCES = \
	base64.c \
	encodings.c \
	queue.c \
	simplestring.c \
	xml_element.c \
	xml_to_dandarpc.c \
	xml_to_xmlrpc.c \
        xml_to_soap.c \
	xmlrpc.c \
	xmlrpc_introspection.c \
        system_methods.c
        
libxmlrpc_la_LDFLAGS = @LIBICONV@ -version-info 0:3:0 

noinst_HEADERS = \
	base64.h \
	encodings.h \
	queue.h \
	simplestring.h \
   system_methods_private.h \
	xml_element.h \
   xml_to_dandarpc.h \
   xml_to_soap.h \
	xml_to_xmlrpc.h \
	xmlrpc.h \
	xmlrpc_introspection.h \
   xmlrpc_introspection_private.h \
   xmlrpc_private.h

SHELL = /bin/sh

DEFS=-DVERSION=\"0.51\"
