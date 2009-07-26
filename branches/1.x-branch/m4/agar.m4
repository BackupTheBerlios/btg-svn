dnl Based on AM_PATH_CPPUNIT.

dnl
dnl AM_PATH_AGAR(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN([AM_PATH_AGAR],
[

AC_ARG_WITH(agar-prefix,[  --with-agar-prefix=PFX   Prefix where Agar is installed (optional)],
            agar_config_prefix="$withval", agar_config_prefix="")
AC_ARG_WITH(agar-exec-prefix,[  --with-agar-exec-prefix=PFX  Exec prefix where Agar is installed (optional)],
            agar_config_exec_prefix="$withval", agar_config_exec_prefix="")

  if test x$agar_config_exec_prefix != x ; then
     agar_config_args="$agar_config_args --exec-prefix=$agar_config_exec_prefix"
     if test x${AGAR_CONFIG+set} != xset ; then
        AGAR_CONFIG=$agar_config_exec_prefix/bin/agar-config
     fi
  fi
  if test x$agar_config_prefix != x ; then
     agar_config_args="$agar_config_args --prefix=$agar_config_prefix"
     if test x${AGAR_CONFIG+set} != xset ; then
        AGAR_CONFIG=$agar_config_prefix/bin/agar-config
     fi
  fi

  AC_PATH_PROG(AGAR_CONFIG, agar-config, no)
  agar_version_min=$1

  AC_MSG_CHECKING(for Agar - version >= $agar_version_min)
  no_agar=""
  if test "$AGAR_CONFIG" = "no" ; then
    no_agar=yes
  else
    AGAR_CFLAGS=`$AGAR_CONFIG --cflags`
    AGAR_LIBS=`$AGAR_CONFIG --libs`
    agar_version=`$AGAR_CONFIG --version`

    agar_major_version=`echo $agar_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    agar_minor_version=`echo $agar_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    agar_micro_version=`echo $agar_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    agar_major_min=`echo $agar_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    agar_minor_min=`echo $agar_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    agar_micro_min=`echo $agar_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    agar_version_proper=`expr \
        $agar_major_version \> $agar_major_min \| \
        $agar_major_version \= $agar_major_min \& \
        $agar_minor_version \> $agar_minor_min \| \
        $agar_major_version \= $agar_major_min \& \
        $agar_minor_version \= $agar_minor_min \& \
        $agar_micro_version \>= $agar_micro_min `

    if test "$agar_version_proper" = "1" ; then
      AC_MSG_RESULT([$agar_major_version.$agar_minor_version.$agar_micro_version])
    else
      AC_MSG_RESULT(no)
      no_agar=yes
    fi
  fi

  if test "x$no_agar" = x ; then
     ifelse([$2], , :, [$2])     
  else
     AGAR_CFLAGS=""
     AGAR_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_SUBST(AGAR_CFLAGS)
  AC_SUBST(AGAR_LIBS)
])



