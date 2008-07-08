dnl
dnl Find the version of gnutls.
dnl

AC_DEFUN([AM_LIBGNUTLS_VERSION],
[dnl
dnl Get the cflags and libraries from the libgnutls-config script
dnl
AC_ARG_WITH(libgnutls-prefix,
          [  --with-libgnutls-prefix=PFX   Prefix where libgnutls is installed (optional)],
          libgnutls_config_prefix="$withval", libgnutls_config_prefix="")

  if test x$libgnutls_config_prefix != x ; then
     if test x${LIBGNUTLS_CONFIG+set} != xset ; then
        LIBGNUTLS_CONFIG=$libgnutls_config_prefix/bin/libgnutls-config
     fi
  fi

  AC_PATH_PROG(LIBGNUTLS_CONFIG, libgnutls-config, no)

  if test "$LIBGNUTLS_CONFIG" = "no" ; then
    LIBGNUTLS_MAJOR_VER=0
    LIBGNUTLS_MINOR_VER=0
  else
    LIBGNUTLS_MAJOR_VER=`$LIBGNUTLS_CONFIG $libgnutls_config_args --version|cut -d "." -f 1`
    LIBGNUTLS_MINOR_VER=`$LIBGNUTLS_CONFIG $libgnutls_config_args --version|cut -d "." -f 2`
  fi

  AC_DEFINE_UNQUOTED(GNUTLS_MAJOR_VER, [$LIBGNUTLS_MAJOR_VER], [gnutls major version])
  AC_DEFINE_UNQUOTED(GNUTLS_MINOR_VER, [$LIBGNUTLS_MINOR_VER], [gnutls minor version])
])

dnl *-*wedit:notab*-*  Please keep this as the last line.
