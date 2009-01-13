AC_DEFUN([AC_LIRCCLIENT],
[
  dnl Check for LIRC client support

  LIRC_CLIENT_LIB="lirc_client"

  have_lirc=yes
  AC_REQUIRE_CPP
  AC_CHECK_HEADER(lirc/lirc_client.h,true,have_lirc=no)

  if test "$have_lirc" = "no"; then
    AC_MSG_ERROR([LIRC client support not available (headers)]);
  fi

  AC_CHECK_LIB($LIRC_CLIENT_LIB,lirc_init,have_lirc=yes,have_lirc=no)

  if test "$have_lirc" = "no"; then
    AC_MSG_ERROR([LIRC client support not available (libs)]);
  fi

  AC_DEFINE(HAVE_LIRC,1,[Define if you have LIRC libraries and header files.])
  
  AC_SUBST(LIRC_CLIENT_LIB)
])
