if BUILDINTERNALXMLRPC
SUBDIRS = expat src 
else
INTERNAL_XMLRPC_HEADER = \
expat/xmlparse/hashtable.h \
expat/xmlparse/xmlparse.h \
expat/xmlwf/filemap.h \
expat/xmlwf/xmlfile.h \
expat/xmlwf/xmltchar.h \
expat/xmlwf/codepage.h \
expat/xmltok/utf8tab.h \
expat/xmltok/xmltok.h \
expat/xmltok/asciitab.h \
expat/xmltok/latin1tab.h \
expat/xmltok/xmldef.h \
expat/xmltok/xmltok_impl.h \
expat/xmltok/xmlrole.h \
expat/xmltok/nametab.h \
expat/xmltok/iasciitab.h \
src/xml_to_dandarpc.h \
src/xmlrpc_private.h \
src/system_methods_private.h \
src/xmlrpc.h \
src/xml_element.h \
src/encodings.h \
src/xmlrpc_introspection_private.h \
src/simplestring.h \
src/queue.h \
src/base64.h \
src/xml_to_soap.h \
src/xmlrpc_introspection.h \
src/xml_to_xmlrpc.h

INTERNAL_XMLRPC_SOURCE = \
expat/gennmtab/gennmtab.c \
expat/xmlparse/hashtable.c \
expat/xmlparse/xmlparse.c \
expat/xmlwf/win32filemap.c \
expat/xmlwf/xmlfile.c \
expat/xmlwf/unixfilemap.c \
expat/xmlwf/codepage.c \
expat/xmlwf/readfilemap.c \
expat/xmlwf/xmlwf.c \
expat/xmltok/xmltok.c \
expat/xmltok/xmltok_ns.c \
expat/xmltok/xmltok_impl.c \
expat/xmltok/dllmain.c \
expat/xmltok/xmlrole.c \
src/xml_to_dandarpc.c \
src/xmlrpc.c \
src/system_methods.c \
src/xml_element.c \
src/encodings.c \
src/simplestring.c \
src/queue.c \
src/base64.c \
src/xml_to_soap.c \
src/xmlrpc_introspection.c \
src/xml_to_xmlrpc.c

EXTRA_DIST = $(INTERNAL_XMLRPC_HEADER) $(INTERNAL_XMLRPC_SOURCE)

endif

