#!/bin/sh

# 
# This script is used to generate the configure script using GNU
# autotools.
# 

# $Id$

set -x
set -e

aclocal -I m4
autoheader
libtoolize -c -f
automake --add-missing --copy --gnu
autoconf

rm -Rf config.cache autom4te.cache *~
