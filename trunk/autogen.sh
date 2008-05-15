#!/bin/sh

# 
# This script is used to generate the configure script using GNU
# autotools.
# 

# $Id$

set -x
set -e

aclocal-1.9 -I m4
autoheader
libtoolize -c -f
automake-1.9 --add-missing --copy --gnu
autoconf

rm -Rf config.cache autom4te.cache *~
