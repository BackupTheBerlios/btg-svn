dnl
dnl Execinfo stuff detection
dnl
dnl
dnl $Id$
dnl

# execinfo detection
AC_DEFUN([BTG_EXECINFO_CHECK],
[
	AC_MSG_CHECKING([for execinfo (backtrace* functions) in libc])
	AC_TRY_LINK([
		#include <execinfo.h>
	],[
		backtrace(0, 0);
		backtrace_symbols(0, 0);
		return 0;
	],[
		AC_MSG_RESULT([yes])
		AC_DEFINE([HAVE_EXECINFO],[1],[Execinfo detected and can be used.])
	],[
		AC_MSG_RESULT([no])
		SAVE_LIBS=$LIBS
		LIBS="$LIBS -lexecinfo"
		AC_MSG_CHECKING([for execinfo in libexecinfo])
		AC_TRY_LINK([
			#include <execinfo.h>
		],[
			backtrace(0, 0);
			backtrace_symbols(0, 0);
			return 0;
		],[
			AC_MSG_RESULT([yes])
			AC_DEFINE([HAVE_EXECINFO],[1],[Execinfo detected and can be used.])
		],[
			AC_MSG_RESULT([no])
			LIBS=$SAVE_LIBS
		])
	])
])
