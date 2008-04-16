#! /bin/sh

# $Id$

set -e # fail on errors
#set -x # debug

version=$1
suffix=$2

[ -z "$version" ] && { echo "USAGE: $0 <version> [suffix]"; exit 1; }
[ -z "$suffix" ] && exit 0

for pkg in btg-common btg-cli-common btg-cli btg-gui btg-www btg-daemon ; do
	for file in install init postinst postrm ; do
		rm -f ${pkg}${suffix}.$file
		[ -e $pkg.$file ] && ln -s $pkg.$file ${pkg}${suffix}.$file
	done
done
