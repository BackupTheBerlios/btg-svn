dnl
dnl Find the version of gnutls.
dnl

AC_DEFUN([AM_LIBGNUTLS_VERSION],
[
  PKG_CHECK_EXISTS(gnutls, [gnutls_exists=yes], [gnutls_exists=no])

  if test "$gnutls_exists" = "no" ; then
    LIBGNUTLS_MAJOR_VER=0
    LIBGNUTLS_MINOR_VER=0
  else
    LIBGNUTLS_MAJOR_VER=`pkg-config gnutls --modversion --silence-errors |cut -d "." -f 1`
    LIBGNUTLS_MINOR_VER=`pkg-config gnutls --modversion --silence-errors |cut -d "." -f 2`
  fi

  AC_DEFINE_UNQUOTED(GNUTLS_MAJOR_VER, [$LIBGNUTLS_MAJOR_VER], [gnutls major version])
  AC_DEFINE_UNQUOTED(GNUTLS_MINOR_VER, [$LIBGNUTLS_MINOR_VER], [gnutls minor version])
])

dnl *-*wedit:notab*-*  Please keep this as the last line.
