AC_DEFUN([AC_LIRCCLIENT],
[
  dnl Check for LIRC client support
  LIRC_OK=0
  LIRC_CLIENT_LIB=""

  have_lirc_header=yes
  dnl AC_REQUIRE_CPP
  AC_CHECK_HEADER(lirc/lirc_client.h,true,have_lirc_header=no)

  if test "$have_lirc_header" = "yes"; then
    AC_MSG_NOTICE([Found LIRC client headers.])
    AC_CHECK_LIB(lirc_client,lirc_init,have_lirc_libs=yes,have_lirc_libs=no)
    if test "$have_lirc_libs" = "yes"; then
      AC_DEFINE(HAVE_LIRC,1,[Define if you have LIRC libraries and header files.])
      LIRC_CLIENT_LIB="-llirc_client"
      AC_MSG_NOTICE([Found LIRC client libs.])
    else
      AC_DEFINE(HAVE_LIRC,0,[Define if you have LIRC libraries and header files.])
      AC_MSG_NOTICE([LIRC client libs not found.])
    fi
  fi

  AC_SUBST(LIRC_CLIENT_LIB)
])
