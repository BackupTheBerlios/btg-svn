dnl @synopsis AC_CXX_VERBOSE_TERMINATE_HANDLER
dnl
dnl If the compiler does have the verbose terminate handler, define
dnl HAVE_VERBOSE_TERMINATE_HANDLER.
dnl
dnl @category Cxx
dnl @author Lapo Luchini <lapo@lapo.it>
dnl @version 2003-01-20
dnl @license AllPermissive

AC_DEFUN([AC_CXX_VERBOSE_TERMINATE_HANDLER],
[AC_CACHE_CHECK(whether the compiler has __gnu_cxx::__verbose_terminate_handler,
ac_cv_verbose_terminate_handler,
[
  AC_REQUIRE([AC_CXX_EXCEPTIONS])
  AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE(
    [#include <exception>], [std::set_terminate(__gnu_cxx::__verbose_terminate_handler);],
    ac_cv_verbose_terminate_handler=yes, ac_cv_verbose_terminate_handler=no
  )
  AC_LANG_RESTORE
])
if test "$ac_cv_verbose_terminate_handler" = yes; then
  AC_DEFINE(HAVE_VERBOSE_TERMINATE_HANDLER, , [define if the compiler has __gnu_cxx::__verbose_terminate_handler])
fi
])
