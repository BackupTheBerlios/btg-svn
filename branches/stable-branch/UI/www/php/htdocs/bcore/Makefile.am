SUBDIRS = command

WWW_UI_DEST  = $(datadir)/wwwbtg/htdocs/bcore
WWW_UI_FILES = addrport.php btgexception.php \
	command_factory.php file_info.php hash.php peer.php \
	sbuf.php serializable.php status.php xmlrpc.php trackerstatus.php
EXTRA_DIST = $(WWW_UI_FILES)

if BUILDWWW
wwwdir = $(WWW_UI_DEST)
www_DATA = $(WWW_UI_FILES)
endif
