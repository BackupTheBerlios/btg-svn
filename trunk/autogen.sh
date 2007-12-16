#!/bin/sh
# 
# This script is used to generate the configure script using GNU
# autotools.
# 

echo "Deleting temp files."

rm -f config.cache
rm -fr autom4te.cache

if test "`uname`" = "FreeBSD"  && test "`uname -r|cut -f1 -d'.'`" != "7"
then
	autoheader259 &&
	aclocal19 -I m4 -I /usr/local/share/aclocal &&
	libtoolize -c -f &&
	automake19 --add-missing --copy --gnu &&
	autoconf259 &&
	autoheader259 &&
	rm -f config.cache
else 
	echo "Running autoheader."
	autoheader
	echo "Running aclocal."
	aclocal -I m4
	echo "Running libtoolize."
	libtoolize -c -f
	echo "Running automake."
	automake --add-missing --copy --gnu
	echo "Running autoconf."
	autoconf
	echo "Running autoheader."
	autoheader
fi
	
echo "Done"
