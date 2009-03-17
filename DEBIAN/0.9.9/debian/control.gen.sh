#! /bin/sh

# $Id$

version=$1
suffix=$2

[ -z "$version" ] && { echo "USAGE: $0 <version> [suffix]"; exit 1; }

sed -e "s/@VERSION@/$version/g;s/@SUFFIX@/$suffix/g;" `dirname $0`/control.in > `dirname $0`/control
