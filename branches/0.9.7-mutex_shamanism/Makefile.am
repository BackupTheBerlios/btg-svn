if BTG_UNITTEST
  BTG_UNITTEST_DIR = utest
endif

SUBDIRS = external bcore daemon UI doc script $(BTG_UNITTEST_DIR) stest

GENTOO_PKG = gentoo/net-libs/rblibtorrent/README \
gentoo/net-libs/rblibtorrent/rblibtorrent.ebuild \
gentoo/net-p2p/btg/btg.ebuild \
gentoo/README

EXTRA_DIST = AUTHORS TODO README configure menuconfig.sh autogen.sh setup.sh

# Cannot use --enable-all flag here, as installation of www ui borkes
# distcheck.
# The reason is that the www ui install rules install data files in
# quite unexpected locations.

DISTCHECK_CONFIGURE_FLAGS = --enable-gui --enable-cli --enable-debug --enable-unittest --enable-session-saving --enable-event-callback

doxygen:
	cd doc && $(MAKE) doxygen

deb:
	dpkg-buildpackage -rfakeroot -us -uc

if !BTG_UNITTEST
check:
	@echo "***"
	@echo "Running unittests disabled."
	@echo "***"
endif

# Run an application written using dialog,
# which shows a list of compile time options
# and if the user wishes runs the configure 
# script.

menuconfig:
	./menuconfig.sh
