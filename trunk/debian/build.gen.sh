#! /bin/sh

# $Id$

set -e # fail on errors

version=$1
suffix=$2

[ -z "$version" ] && { echo "USAGE: $0 <version> [suffix]"; exit 1; }

for s in control.gen.sh changelog.add.pl debfiles.gen.sh ; do
	./$s $version $suffix
done
