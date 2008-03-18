#!/bin/sh

# 
# Script used to setup BTG to use a certain libtorrent version.
# Written to make it easier to switch between versions.
# 
# Usage: ./setup.sh libtorrent_version
# Example ./setup svn -optimize.
# 
#
# Note: This is for developement use only. Read the howto for
# information about how to build/setup BTG.
#
# 
# The root of the path where libtorrent is installed.
# 
ROOT=/pack/libtorrent

GUI_CLIENT=""

# If gtkmm-config is present, the headers should be 
# present as well.

if [ -f "/usr/include/gtkmm-2.4/gtkmm.h" ]
then
  echo "Building GUI client."
  GUI_CLIENT="--enable-gui"
else
  echo "NOT building GUI client."
fi

AGAR_CONFIG=`type -p agar-config`

if [ -f "$AGAR_CONFIG" ]
then
  echo "Building GUI viewer."
  GUI_VIEWER=--enable-viewer
else
  echo "NOT building GUI viewer."
fi

for arg in "$@"
do
    case "$arg" in
	"-profile")
	    echo "Enabling: profiled build."
	    export CXXFLAGS="$CXXFLAGS -g -pg"
	    export CFLAGS="$CFLAGS -g -pg"
	    ;;
	
	"-optimize")
	    echo "Enabling: optimized build."
	    export CXXFLAGS="$CXXFLAGS -O3"
	    export CFLAGS="$CFLAGS -O3"
	    ;;
	"-debug")
	    echo "Enabling: debug build."
	    DEBUG=--enable-debug
	    export CXXFLAGS="$CXXFLAGS -g"
	    export CFLAGS="$CFLAGS -g"
	    ;;
	"-ccache")
	    echo "Enabling: using ccache."
	    export CXX="ccache g++"
	    export CC="ccache gcc"
	    ;;
	"-static")
	    echo "Enabling static."
	    export STATIC="--enable-shared=no --enable-static=yes"
	    ;;
	"-help")
	    echo "Help:"
	    echo "Argument 0:"
	    echo "libtorrent version: \"0.12\" \"svn\" \"system\""
	    echo "Argument 1:"
	    echo "-profile  - add profiling"
	    echo "-debug    - add debug" 
	    echo "-optimize - optimize"
	    echo "-ccache   - use ccache"
	    ;;
    esac
done

echo "Using CXXFLAGS: $CXXFLAGS"
echo "Using CFLAGS: $CFLAGS"

# 
# The configure command to execute after setting CFLAGS etc.
# 

# Tell the configure script which versions of the boost libs to use.
BOOST_SUFFIX="gcc41-mt-1_34_1"

# Use a certain boost suffix. Hopefully it will stay the same on GNU/Debian.
CONFIGURE_BOOST="--with-boost-date-time=$BOOST_SUFFIX --with-boost-filesystem=$BOOST_SUFFIX --with-boost-thread=$BOOST_SUFFIX --with-boost-regex=$BOOST_SUFFIX --with-boost-program_options=$BOOST_SUFFIX --with-boost-iostreams=$BOOST_SUFFIX"

# Execute this configure command.
CONFIGURE="./configure $STATIC --disable-static $DEBUG --enable-btg-config --enable-cli $GUI_CLIENT $GUI_VIEWER --enable-unittest --enable-session-saving --enable-command-list --enable-event-callback --enable-upnp --enable-www --prefix=/pack/btg-cvs $CONFIGURE_BOOST"

case "$1" in
  0.13)
    export LIBTORRENT_CFLAGS="-I$ROOT/$1/include -I$ROOT/$1/include/libtorrent" && \
    export LIBTORRENT_LIBS="-L$ROOT/$1/lib -ltorrent" && \
    echo "Using $CONFIGURE";
    $CONFIGURE --with-rblibtorrent=$ROOT/$1
    ;;
  0.13rc2)
    export LIBTORRENT_CFLAGS="-I$ROOT/$1/include -I$ROOT/$1/include/libtorrent" && \
    export LIBTORRENT_LIBS="-L$ROOT/$1/lib -ltorrent" && \
    echo "Using $CONFIGURE";
    $CONFIGURE --with-rblibtorrent=$ROOT/$1
    ;;
  0.12.1)
    export LIBTORRENT_CFLAGS="-I$ROOT/$1/include -I$ROOT/$1/include/libtorrent" && \
    export LIBTORRENT_LIBS="-L$ROOT/$1/lib -ltorrent" && \
    echo "Using $CONFIGURE";
    $CONFIGURE --with-rblibtorrent=$ROOT/$1
    ;;
  svn)
    export LIBTORRENT_CFLAGS="-I$ROOT/svn/include -I$ROOT/svn/include/libtorrent" && \
    export LIBTORRENT_LIBS="-L$ROOT/svn/lib -ltorrent" && \
    echo "Using $CONFIGURE";
    $CONFIGURE --with-rblibtorrent=$ROOT/svn
    ;;
  system)
    # Use libtorrent installed system wide.
    $CONFIGURE
    ;;
  *)
    echo "Unknown libtorrent version."
    exit -1
    echo "Possible arguments: \"0.12\" \"svn\" \"system\""
    ;;
esac



