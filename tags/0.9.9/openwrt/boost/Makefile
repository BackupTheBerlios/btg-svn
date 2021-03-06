# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 9173 2007-10-07 03:20:26Z blogic $

include $(TOPDIR)/rules.mk

PKG_NAME:=boost
PKG_VERSION:=1_34_1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://heanet.dl.sourceforge.net/sourceforge/boost/
PKG_MD5SUM:=2d938467e8a448a2c9763e0a9f8ca7e5
#PKG_BUILD_DIR:=$(PKG_NAME)_$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)_$(PKG_VERSION)
#PKG_BUILD_DEPENDS:=libtool

include $(INCLUDE_DIR)/package.mk

define Package/boost
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Boost
  URL:=http://boost.org/
  DEPENDS:=+bzip2 +zlib +libpthread +libstdcpp
endef

define Package/boost/description
 Boost provides free peer-reviewed portable C++ source libraries.
endef

define Build/Configure
	echo "import toolset.using ;" > ~/user-config.jam
	# echo "using gcc : : g++-uc+std ;" >> ~/user-config.jam
	echo "using gcc : : $(GNU_TARGET_NAME)-g++ : <cxxflags>\"-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include\" <linkflags>\"-lc -gcc\" ; " >> ~/user-config.jam

endef

define Build/Compile
	export CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include"
	export LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib"
	cd $(PKG_BUILD_DIR) && \
	bjam \
	-d+2 --toolset=gcc --sBUILD=release \
	link=shared \
	-sZLIB_INCLUDE=$(STAGING_DIR)/usr/include \
	-sZLIB_LIBPATH=$(STAGING_DIR)/usr/lib \
	--prefix=$(PKG_INSTALL_DIR) \
	--with-thread --with-filesystem --with-date_time \
	--with-program_options --with-serialization --with-iostreams install
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/include/boost-1_34_1/boost $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/* $(1)/usr/lib/
endef

define Build/UninstallDev
	rm -rf	$(STAGING_DIR)/usr/include/boost \
		$(STAGING_DIR)/usr/lib/libboost*
endef

define Package/boost/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,boost))

