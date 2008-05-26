dnl
dnl Rasterbar Libtorrent detection
dnl
dnl
dnl $Id$
dnl

AC_DEFUN([BTG_RBLIBTORRENT_CHECK],
[
    AC_ARG_WITH(rblibtorrent, AC_HELP_STRING([--with-rblibtorrent], [Where Rasterbar libtorrent is installed]),
    [          
        rblibtorrent_prefix="$withval"

        for dir in $rblibtorrent_prefix; do
    	    libtorrentdir="$dir"
    	    if test -f "$dir/include/libtorrent/version.hpp"; then
            	found_rblibtorrent_header="yes";
                RBLIBTORRENT_ROOT="$libtorrentdir"
    	        break;
    	    fi
        done

	if test x_$found_rblibtorrent_header != x_yes; then
    	    AC_MSG_ERROR([Cannot find Rasterbar Libtorrent headers])
        fi

	AC_MSG_NOTICE([Searching for libtorrent in $RBLIBTORRENT_ROOT (by pkgconfig) ...])
	PKG_CONFIG_PATH_saved=$PKG_CONFIG_PATH
	export PKG_CONFIG_PATH="$RBLIBTORRENT_ROOT/lib/pkgconfig:$RBLIBTORRENT_ROOT/libdata/pkgconfig"
        PKG_CHECK_MODULES([LIBTORRENT], [libtorrent >= 0.12], [], [
		AC_MSG_NOTICE([Trying to use the default libtorrent CFLAGS/LIBS.])
        	LIBTORRENT_CFLAGS="-I$RBLIBTORRENT_ROOT/include -I$RBLIBTORRENT_ROOT/include/libtorrent"
		LIBTORRENT_LIBS="-L$RBLIBTORRENT_ROOT/lib -ltorrent"
		AC_SUBST(LIBTORRENT_CFLAGS)
		AC_SUBST(LIBTORRENT_LIBS)
	])
	export PKG_CONFIG_PATH=$PKG_CONFIG_PATH_saved
	
        CXXFLAGS_SAVED="$CXXFLAGS"
	CXXFLAGS="$CXXFLAGS $LIBTORRENT_CFLAGS"
        LIBS_SAVED="$LIBS"
    	LIBS="$LIBS $LIBTORRENT_LIBS"
        AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

    	AC_MSG_CHECKING([whether Rasterbar Libtorrent headers can be used])
        AC_COMPILE_IFELSE([
    	    AC_LANG_PROGRAM([
#       	include <libtorrent/session.hpp>
	    ], [
    		libtorrent::session s(libtorrent::fingerprint("LT", 0, 1, 2, 0));
	        return 0;
    	    ])
	    ], [
                AC_MSG_RESULT([yes])
	    ], [
	        AC_MSG_RESULT([no])
                AC_MSG_ERROR([Cannot use Rasterbar Libtorrent headers from the specified libtorrent location])
	    ])

        AC_MSG_CHECKING([whether Rasterbar Libtorrent library can be used])
    	AC_TRY_LINK([
#	        include <libtorrent/session.hpp>
    	], [
	    libtorrent::session s(libtorrent::fingerprint("LT", 0, 1, 2, 0));
            return 0;
    	], [
            AC_MSG_RESULT([yes])
    	], [
            AC_MSG_RESULT([no])
            AC_MSG_ERROR([Cannot link with Rasterbar Libtorrent from the specified libtorrent location])
	])

        AC_LANG_RESTORE
        CXXFLAGS="$CXXFLAGS_SAVED"
        LIBS="$LIBS_SAVED"

        AC_SUBST(LIBTORRENT_CFLAGS)
        AC_SUBST(LIBTORRENT_LIBS)
    ], [
        dnl else try to use the settings from the libtorrent pkg-config configuration
        PKG_CHECK_MODULES([LIBTORRENT], [libtorrent >= 0.12])
    ])
])
