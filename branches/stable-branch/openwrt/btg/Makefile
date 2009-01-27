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
PKG_REV:=532
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://svn.berlios.de/svnroot/repos/btg/trunk
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=svn

include $(INCLUDE_DIR)/package.mk

define Package/btg
  SUBMENU:=BitTorrent
  SECTION:=net
  CATEGORY:=Network
  TITLE:=BTG - a bittorrent daemon
  URL:=btg.berlios.de
  DEPENDS:=+rblibtorrent +libiconv +libexpat +gnutls +libcurl
endef

define Package/btg/description
 BTG is a bittorrent daemon.
endef

CONFIGURE_ARGS+= \
	--disable-static \
	--disable-debug \
	--enable-session-saving \
	--enable-url \
	--with-boost-date-time=gcc42-mt-1_34_1 \
	--with-boost-filesystem=gcc42-mt-1_34_1 \
	--with-boost-thread=gcc42-mt-1_34_1 \
	--with-boost-regex=gcc42-mt-1_34_1 \
	--with-boost-program_options=gcc42-mt-1_34_1 \
	--with-boost-iostreams=gcc42-mt-1_34_1 \
	--with-boost-program_options=gcc42-mt-1_34_1 \
	--with-rblibtorrent=$(STAGING_DIR)/usr \

CONFIGURE_VARS += \
	CC="$(GNU_TARGET_NAME)-gcc" \
	CXX="$(GNU_TARGET_NAME)-g++" \
	CXXFLAGS="$$$$CXXFLAGS -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	CFLAGS="$$$$CFLAGS -I$(STAGING_DIR)/usr/lib/libiconv/include -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	LDFLAGS="$$$$LDFLAGS -Wl,-rpath=$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/usr/lib/libiconv/lib -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib -lz -lbz2 -lpthread -lboost_iostreams-gcc42-mt-1_34_1 -lboost_filesystem-gcc42-mt-1_34_1 -lboost_date_time-gcc42-mt-1_34_1 -lboost_thread-gcc42-mt-1_34_1 -lboost_program_options-gcc42-mt-1_34_1 -lssl -lcrypto -liconv -lcurl" \

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
#	$(CP) $(PKG_INSTALL_DIR)/usr/bin/btg-config $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib/btg
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/btg/libbtgcore.so* $(1)/usr/lib/btg
endef

$(eval $(call BuildPackage,btg))

