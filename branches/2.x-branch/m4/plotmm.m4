AC_DEFUN([PLOTMM_CHECK],
[
  AC_MSG_CHECKING([for PlotMM])

  PREFIXSET=0
  AC_ARG_WITH([plotmm-prefix],
[  --with-plotmm-prefix=DIR  search for plotmm in DIR/include and DIR/lib], [
    for dir in `echo "$withval" | tr : ' '`; do
      if test -d $dir/include; then PLOTMM_CFLAGS="-I$dir/include"; fi
      if test -d $dir/lib; then PLOTMM_LDFLAGS="-L$dir/lib"; fi
    done
    PREFIXSET=1
   ])

  if test "$PREFIXSET" -eq "0"; then
    PKG_CHECK_EXISTS(plotmm >= 0.1.2, [
      PKG_CHECK_MODULES(PLOTMM, plotmm >= 0.1.2, [
      dnl AC_SUBST(PLOTMM_CFLAGS)
      PLOTMM_LDFLAGS="$PLOTMM_LIBS"
    ], [])
    ], [])
  else
    PLOTMM_LIBNAME="plotmm"
    PLOTMM_LDFLAGS="$PLOTMM_LDFLAGS -l$PLOTMM_LIBNAME"
  fi


  CXXFLAGS_SAVED="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS $PLOTMM_CFLAGS"
  LDFLAGS_SAVED="$LDFLAGS"
  LDFLAGS="$LDFLAGS $PLOTMM_LDFLAGS"

  AC_MSG_NOTICE("Using CXXFLAGS: $CXXFLAGS")
  AC_MSG_NOTICE("Using LDFLAGS: $LDFLAGS")

  AC_MSG_CHECKING([whether PlotMM headers can be used])
  AC_COMPILE_IFELSE([
  AC_LANG_PROGRAM([
    # include <plotmm/plot.h>
  ], [
    PlotMM::Plot();
    return 0;
  ])
  ], [
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
    AC_MSG_NOTICE([Cannot use PlotMM headers from the specified location])
  ])

  AC_MSG_CHECKING([whether PlotMM library can be used])
  AC_TRY_LINK([
    # include <plotmm/plot.h>
  ], [
  	 PlotMM::Plot();
    return 0;
  ], [
    AC_MSG_RESULT([yes])
    AC_SUBST(PLOTMM_CXXFLAGS)
    AC_SUBST(PLOTMM_LDFLAGS)
  ], [
    AC_MSG_RESULT([no])
    PLOTMM_LDFLAGS=""
    PLOTMM_CXXFLAGS=""
    AC_MSG_NOTICE([Cannot link with PlotMM from the specified location])
  ])

  CXXFLAGS="$CXXFLAGS_SAVED"
  LDFLAGS="$LDFLAGS_SAVED"
])

