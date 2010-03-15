# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 9173 2007-10-07 03:20:26Z blogic $

include $(TOPDIR)/rules.mk

# Build using trunk from btg SVN.

PKG_NAME:=btg
PKG_REV:=642
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://svn.berlios.de/svnroot/repos/btg/trunk
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=svn
BOOSTPOSTFIX=gcc43-mt-1_38

include $(INCLUDE_DIR)/package.mk

define Package/btg
  SUBMENU:=BitTorrent
  SECTION:=net
  CATEGORY:=Network
  TITLE:=BTG - a bittorrent daemon
  URL:=btg.berlios.de
  DEPENDS:=+rblibtorrent +libiconv +libexpat +libgnutls +libcurl
endef

define Package/btg/description
 BTG is a bittorrent daemon.
endef

CONFIGURE_ARGS+= \
	--disable-static \
	--disable-debug \
	--enable-session-saving \
	--enable-url \
	--with-boost=$(STAGING_DIR)/usr \
	--with-boost-date-time=$(BOOSTPOSTFIX) \
	--with-boost-filesystem=$(BOOSTPOSTFIX) \
	--with-boost-thread=$(BOOSTPOSTFIX) \
	--with-boost-regex=$(BOOSTPOSTFIX) \
	--with-boost-program_options=$(BOOSTPOSTFIX) \
	--with-boost-iostreams=$(BOOSTPOSTFIX) \
	--with-boost-program_options=$(BOOSTPOSTFIX) \
	--with-rblibtorrent=$(STAGING_DIR)/usr \
	--with-libiconv-prefix=$(STAGING_DIR)/usr \
	--with-zlib=$(STAGING_DIR)/usr \
	--oldincludedir=$(STAGING_DIR)/usr/include \
	--enable-cli \

CONFIGURE_VARS += \
	CC="$(GNU_TARGET_NAME)-gcc" \
	CXX="$(GNU_TARGET_NAME)-g++" \
	CXXFLAGS="$$$$CXXFLAGS -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	CFLAGS="$$$$CFLAGS -I$(STAGING_DIR)/usr/lib/libiconv/include -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	BOOST_CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
	BOOST_LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
	LIBTORRENT_CFLAGS="-I$(STAGING_DIR)/usr/include" \
	LIBTORRENT_LIBS="-L$(STAGING_DIR)/usr/lib -ltorrent-rasterbar" \
	LDFLAGS="$$$$LDFLAGS -Wl,-rpath=$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/usr/lib/libiconv/lib -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib -lz -lbz2 -lpthread \
-lboost_iostreams-$(BOOSTPOSTFIX) \
-lboost_iostreams-$(BOOSTPOSTFIX) \
-lboost_filesystem-$(BOOSTPOSTFIX) \
-lboost_date_time-$(BOOSTPOSTFIX) \
-lboost_thread-$(BOOSTPOSTFIX) \
-lboost_program_options-$(BOOSTPOSTFIX) \
-lssl -lcrypto -liconv -lcurl" \
	LIBGNUTLS_CONFIG=$(STAGING_DIR)/usr/bin/libgnutls-config \

define Build/Configure
	cd $(PKG_BUILD_DIR) && ./autogen.sh && \
	$(call Build/Configure/Default,)
endef

define Build/Compile

	$(MAKE) -C $(PKG_BUILD_DIR) all
#	$(call libtool_fixup_libdir,$(PKG_INSTALL_DIR))
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR="$(PKG_INSTALL_DIR)" install
endef

define Build/InstallDev
#	mkdir -p $(1)/usr/include
#	$(CP) $(PKG_INSTALL_DIR)/usr/include/libtorrent $(1)/usr/include/
#	mkdir -p $(1)/usr/lib
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so $(1)/usr/lib/
#	mkdir -p $(1)/usr/lib/pkgconfig
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libtorrent.pc $(1)/usr/lib/pkgconfig/
#	$(SED) 's,$(TARGET_LDFLAGS),,g' $(1)/usr/lib/pkgconfig/libtorrent.pc
endef

define Build/UninstallDev
#	rm -rf	$(STAGING_DIR)/usr/include/torrent \
#		$(STAGING_DIR)/usr/lib/libtorrent.{a,so*} \
#		$(STAGING_DIR)/usr/lib/pkgconfig/libtorrent.pc
endef

define Package/btg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/btgdaemon $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/btgpasswd $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/btgcli $(1)/usr/bin/
#	$(CP) $(PKG_INSTALL_DIR)/usr/bin/btg-config $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib/btg
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/btg/libbtgclient.so* $(1)/usr/lib/btg
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/btg/libbtgcore.so* $(1)/usr/lib/btg
endef

$(eval $(call BuildPackage,btg))

