dnl
dnl UPnP detection
dnl
dnl
dnl $Id$
dnl


dnl uses LIBTORRENT_CFLAGS & LIBTORRENT_LIBS
dnl so need to be used AFTER libtorrent detection
AC_DEFUN([BTG_UPNP_CHECK],
[
	AC_ARG_ENABLE(upnp,
	[  --enable-upnp           enable the use of UPNP (Cyberlink or native libtorrent implementation).],
	[
		case "${enableval}" in
			yes) enableupnp=true ;;
			no)  enableupnp=false ;;
			*) AC_MSG_ERROR(bad value ${enableval} for --enable-upnp) ;;
		esac
	],
	[enableupnp=false])

	BTG_OPTION_USECYBERLINK=0
	AC_SUBST(BTG_OPTION_USECYBERLINK)
	AM_CONDITIONAL(BUILDCYBERLINK, false)
	BTG_OPTION_USELTUPNP=0
	AC_SUBST(BTG_OPTION_USELTUPNP)
	AM_CONDITIONAL(BUILDLTUPNP, false)

	if $enableupnp ; then
		AC_MSG_CHECKING([for libtorrent UPnP])

		SAVE_CPPFLAGS=$CPPFLAGS
		CPPFLAGS="$CPPFLAGS $LIBTORRENT_CFLAGS"
		SAVE_LIBS=$LIBS
		LIBS="$LIBS $LIBTORRENT_LIBS"
		AC_TRY_LINK([
#			include <libtorrent/upnp.hpp>
		],[
#			if (LIBTORRENT_VERSION_MAJOR == 0) && (LIBTORRENT_VERSION_MINOR == 14)
				libtorrent::upnp upnp;
#			else
#				error "LT version < 0.14 - doesn't support UPnP"
#			endif
			return 0;
		],[
			BTG_OPTION_USELTUPNP=1
			AM_CONDITIONAL(BUILDLTUPNP, true)

			AC_MSG_RESULT([yes])

			CPPFLAGS=$SAVE_CPPFLAGS
			LIBS=$SAVE_LIBS
		],[
			AC_MSG_RESULT([no])

			CPPFLAGS=$SAVE_CPPFLAGS
			LIBS=$SAVE_LIBS

			AC_ARG_WITH(cyberlink-include, AC_HELP_STRING([--with-cyberlink-include=PATH], [path to Cyberlink header files]),
			[
				CYBERLINK_CFLAGS="$CFLAGS -I${withval} -DHAVE_SOCKLEN_T"
				AC_SUBST(CYBERLINK_CFLAGS)

				AM_CONDITIONAL(BUILDCYBERLINK, true)
				BTG_OPTION_USECYBERLINK=1
			])

			AC_ARG_WITH(cyberlink-lib, AC_HELP_STRING([--with-cyberlink-lib=PATH], [path to Cyberlink libraries]),
			[
				CYBERLINK_LIBS="-L${withval} ${withval}/libclink.a -lxerces-c -lxerces-depdom"
				AC_SUBST(CYBERLINK_LIBS)

				AM_CONDITIONAL(BUILDCYBERLINK, true)
				BTG_OPTION_USECYBERLINK=1
			])

			if test "$BTG_OPTION_USECYBERLINK" -ne "1" ; then
				# try to detect our one
				PKG_CHECK_MODULES(CYBERLINK, [libclinkcc], [
					AM_CONDITIONAL(BUILDCYBERLINK, true)
					BTG_OPTION_USECYBERLINK=1
					CYBERLINK_CFLAGS="$CYBERLINK_CFLAGS -DHAVE_SOCKLEN_T"
				], [
					AC_MSG_NOTICE([can't find CyberLink library ($CYBERLINK_PKG_ERRORS)])
				])
			fi

		]); dnl AC_TRY_LINK LT-UPnP

	fi dnl if $enableupnp
])
