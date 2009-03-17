ACLOCAL_AMFLAGS = -I m4

if BTG_UNITTEST
  BTG_UNITTEST_DIR = utest
endif

SUBDIRS = external bcore daemon UI doc script $(BTG_UNITTEST_DIR) stest

GENTOO_PKG = gentoo/net-libs/rblibtorrent/README \
gentoo/net-libs/rblibtorrent/rblibtorrent.ebuild \
gentoo/net-p2p/btg/btg.ebuild \
gentoo/README

EXTRA_DIST = AUTHORS TODO README configure menuconfig.sh autogen.sh setup.sh \
	gen_all_builds.pl gen_all_builds_launch.pl

# Taking it from the environment
# DISTCHECK_CONFIGURE_FLAGS = --with-boost-iostreams=mt --with-boost-filesystem=mt --with-boost-date-time=mt --with-boost-thread=mt --with-boost-program-options=mt

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
