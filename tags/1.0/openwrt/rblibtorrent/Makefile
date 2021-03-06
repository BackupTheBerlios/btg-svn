# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 9173 2007-10-07 03:20:26Z blogic $

include $(TOPDIR)/rules.mk

PKG_NAME:=rblibtorrent
PKG_VERSION:=0.14.1
PKG_RELEASE:=1

PKG_SOURCE:=libtorrent-rasterbar-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://dfn.dl.sourceforge.net/sourceforge/libtorrent/
PKG_MD5SUM:=6aa0ed4da604b8b160bf12bcd41d5055
PKG_BUILD_DIR:=$(BUILD_DIR)/libtorrent-rasterbar-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=libtool
#BOOSTPOSTFIX=mt-1_38
BOOSTPOSTFIX=gcc41-mt-1_38

include $(INCLUDE_DIR)/package.mk

define Package/rblibtorrent
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Rasterbar BitTorrent library
  URL:=http://www.rasterbar.com/products/libtorrent/
  DEPENDS:=+boost +libopenssl 
endef

define Package/rblibtorrent/description
 Rasterbar libtorrent is a C++ library that aims to be a good alternative to all the other bittorrent implementations around. It is a library and not a full featured client, although it comes with a working example client.
endef

CONFIGURE_ARGS+= \
	--enable-shared \
	--disable-debug \
	--with-ssl=$(STAGING_DIR)/usr/include \
	--with-boost=$(STAGING_DIR)/usr/include \
	--with-boost-libdir=$(STAGING_DIR)/usr/lib \
	--with-zlib=system \
	--with-boost-system=$(BOOSTPOSTFIX) \
	--with-boost-date-time=$(BOOSTPOSTFIX) \
	--with-boost-filesystem=$(BOOSTPOSTFIX) \
	--with-boost-thread=$(BOOSTPOSTFIX) \
	--with-boost-regex=$(BOOSTPOSTFIX) \
	--with-boost-program_options=$(BOOSTPOSTFIX) \
	--with-boost-iostreams=$(BOOSTPOSTFIX) \

CFLAGS="$(TARGET_CFLAGS)"
CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include"
#BOOST_CPPFLAGS="$(STAGING_DIR)/usr/include"

CONFIGURE_VARS += \
	CC="$(GNU_TARGET_NAME)-gcc" \
	CXX="$(GNU_TARGET_NAME)-g++" \
	CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	CXXFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$$$$LDFLAGS -Wl,-rpath=$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib -lz -lbz2 -lpthread" \

define Build/Configure
	#cd $(PKG_BUILD_DIR) && \
	#grep -v "AC_CHECK_FILES" configure.in > configure.in.new && \
	#mv configure.in.new configure.in && \
	#autoconf && automake && echo "Removed AC_CHECK_FILES from configure.in"
	$(call Build/Configure/Default,)
endef

define Build/Compile

	$(MAKE) -C $(PKG_BUILD_DIR) all
#	$(call libtool_fixup_libdir,$(PKG_INSTALL_DIR))
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR="$(PKG_INSTALL_DIR)" install
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libtorrent $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so $(1)/usr/lib/
	mkdir -p $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libtorrent-rasterbar.pc $(1)/usr/lib/pkgconfig/
endef

define Build/UninstallDev
#	rm -rf	$(STAGING_DIR)/usr/include/torrent \
#		$(STAGING_DIR)/usr/lib/libtorrent.{a,so*} \
#		$(STAGING_DIR)/usr/lib/pkgconfig/libtorrent-rasterbar.pc
endef

define Package/rblibtorrent/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,rblibtorrent))

