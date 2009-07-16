AC_DEFUN([XMLRPC_CHECK],
[
  AC_MSG_CHECKING([for XML-RPC])

  AC_ARG_WITH([xmlrpc-prefix],
[  --with-xmlrpc-prefix=DIR  search for xmlrpc in DIR/include and DIR/lib], [
    for dir in `echo "$withval" | tr : ' '`; do
      if test -d $dir/include; then XMLRPC_CFLAGS="-I$dir/include"; fi
      if test -d $dir/lib; then XMLRPC_LIBS="-L$dir/lib"; fi
    done
   ])

  XMLRPC_LIBS="$XMLRPC_LIBS -lxmlrpc-epi"

  CFLAGS_SAVED="$CFLAGS"
  CFLAGS="$CFLAGS $XMLRPC_CFLAGS"
  LIBS_SAVED="$LIBS"
  LIBS="$LIBS $XMLRPC_LIBS"

  AC_HEADER_STDC
  dnl AC_CHECK_HEADERS(xmlrpc.h)

  AC_MSG_CHECKING([whether XML-RPC headers can be used])
  AC_COMPILE_IFELSE([
  AC_LANG_PROGRAM([
    # include <xmlrpc.h>
  ], [
    XMLRPC_REQUEST xmlrpc_request = XMLRPC_RequestNew();
    return 0;
  ])
  ], [
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
    AC_MSG_NOTICE([Cannot use XML-RPC headers from the specified location])

    if test -z "$XMLRPC_CFLAGS"; then
      XMLRPC_CFLAGS="-I$prefix/include/xmlrpc-epi"
    else
      XMLRPC_CFLAGS="$XMLRPC_CFLAGS/xmlrpc-epi"
    fi

    CFLAGS="$CFLAGS $XMLRPC_CFLAGS"
    AC_MSG_CHECKING([whether XML-RPC headers can be used in xmlrpc directory])
    AC_COMPILE_IFELSE([
    AC_LANG_PROGRAM([
      # include <xmlrpc.h>
    ], [
      XMLRPC_REQUEST xmlrpc_request = XMLRPC_RequestNew();
      return 0;
    ])
    ], [
      AC_MSG_RESULT([yes])
    ], [
      AC_MSG_RESULT([no])
      AC_MSG_NOTICE([Cannot use XML-RPC headers])
    ])
  ])

  AC_MSG_CHECKING([whether XML-RPC library can be used])
  AC_TRY_LINK([
    # include <xmlrpc.h>
  ], [
    XMLRPC_REQUEST xmlrpc_request = XMLRPC_RequestNew();
    return 0;
  ], [
    AC_MSG_RESULT([yes])
    AC_SUBST(XMLRPC_CFLAGS)
    AC_SUBST(XMLRPC_LIBS)
  ], [
    AC_MSG_RESULT([no])
    XMLRPC_LIBS=""
    XMLRPC_CFLAGS=""
    AC_MSG_NOTICE([Cannot link with XML-RPC from the specified location])
  ])

  CFLAGS="$CFLAGS_SAVED"
  LIBS="$LIBS_SAVED"
])

