AC_DEFUN([XMLRPC_CHECK],
[
  AC_MSG_CHECKING([for XML-RPC])

  AC_ARG_WITH([xmlrpc-prefix],
[  --with-xmlrpc-prefix=DIR  search for xmlrpc in DIR/include and DIR/lib], [
    for dir in `echo "$withval" | tr : ' '`; do
      if test -d $dir/include; then XMLRPC_CFLAGS="-I$dir/include"; fi
      if test -d $dir/lib; then XMLRPC_LDFLAGS="-L$dir/lib"; fi
    done
   ])

  XMLRPC_LIBNAME="xmlrpc-epi"
  XMLRPC_LDFLAGS="$XMLRPC_LDFLAGS -l$XMLRPC_LIBNAME"

  CFLAGS_SAVED="$CFLAGS"
  CFLAGS="$CFLAGS $XMLRPC_CFLAGS"
  LDFLAGS_SAVED="$LDFLAGS"
  LDFLAGS="$LDFLAGS $XMLRPC_LDFLAGS"

  AC_HEADER_STDC
  dnl AC_CHECK_HEADERS(xmlrpc.h)

  AC_MSG_CHECKING([whether XML-RPC headers can be used])
  AC_COMPILE_IFELSE([
  AC_LANG_PROGRAM([
    # include <xmlrpc-epi/xmlrpc.h>
  ], [
    XMLRPC_REQUEST xmlrpc_request = XMLRPC_RequestNew();
    return 0;
  ])
  ], [
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
    AC_MSG_NOTICE([Cannot use XML-RPC headers from the specified location])
  ])

  AC_MSG_CHECKING([whether XML-RPC library can be used])
  AC_TRY_LINK([
    # include <xmlrpc-epi/xmlrpc.h>
  ], [
    XMLRPC_REQUEST xmlrpc_request = XMLRPC_RequestNew();
    return 0;
  ], [
    AC_MSG_RESULT([yes])
    AC_SUBST(XMLRPC_CFLAGS)
    AC_SUBST(XMLRPC_LDFLAGS)
  ], [
    AC_MSG_RESULT([no])
    XMLRPC_LDFLAGS=""
    XMLRPC_CFLAGS=""
    AC_MSG_NOTICE([Cannot link with XML-RPC from the specified location])
  ])

  CFLAGS="$CFLAGS_SAVED"
  LDFLAGS="$LDFLAGS_SAVED"
])

