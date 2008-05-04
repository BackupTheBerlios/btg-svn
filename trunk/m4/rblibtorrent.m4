AC_DEFUN([RBLIBTORRENT],
[
    AC_ARG_WITH(rblibtorrent,
          [  --with-rblibtorrent     Where Rasterbar libtorrent is installed],
          rblibtorrent_prefix="$withval", rblibtorrent_prefix="")

    for dir in $rblibtorrent_prefix /usr /usr/local; do
        libtorrentdir="$dir"
        if test -f "$dir/include/libtorrent/version.hpp"; then
            found_rblibtorrent_header="yes";
            RBLIBTORRENT_ROOT="$libtorrentdir"
            break;
        fi
    done

    if test x_$found_rblibtorrent_header != x_yes; then
        AC_MSG_ERROR(Cannot find Rasterbar Libtorrent headers)
    fi

    CXXFLAGS_SAVED="$CXXFLAGS"
    CXXFLAGS="$CXXFLAGS -I$RBLIBTORRENT_ROOT/include -I$RBLIBTORRENT_ROOT/include/libtorrent"
    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS -L$RBLIBTORRENT_ROOT/lib"
    LIBS_SAVED="$LIBS"
    LIBS="$LIBS -L$RBLIBTORRENT_ROOT/lib -ltorrent"

    AC_MSG_CHECKING([whether Rasterbar Libtorrent headers can be used])

    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[#include <libtorrent/session.hpp>]],
                     [[libtorrent::session s(libtorrent::fingerprint("LT", 0, 1, 2, 0));
                     return 0;]]),
                     rblibtorrent_compiles=yes, rblibtorrent_compiles=no)

    if test x_$rblibtorrent_compiles != x_yes; then
      AC_MSG_RESULT([no])
    else
      AC_MSG_RESULT([yes])
    fi
    

    AC_MSG_CHECKING([whether Rasterbar Libtorrent library can be used])

    AC_TRY_LINK([#include <libtorrent/session.hpp>],
                [libtorrent::session s(libtorrent::fingerprint("LT", 0, 1, 2, 0));
                 return 0;],
                [rblibtorrent_links=yes])

   if test x_$rblibtorrent_links != x_yes; then
     AC_MSG_RESULT([no])
   else
     AC_MSG_RESULT([yes])
   fi

   AC_LANG_RESTORE
   CXXFLAGS="$CXXFLAGS_SAVED"
   LDFLAGS="$LDFLAGS_SAVED"
   LIBS="$LIBS_SAVED"

   if test x_$rblibtorrent_compiles != x_yes; then
     AC_MSG_ERROR(Cannot find Rasterbar Libtorrent headers)
   fi

   if test x_$rblibtorrent_links != x_yes; then
     AC_MSG_ERROR(Cannot link with Rasterbar Libtorrent)
   fi

])dnl
