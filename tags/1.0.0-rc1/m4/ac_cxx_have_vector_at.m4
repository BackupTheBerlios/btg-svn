dnl @synopsis AC_CXX_HAVE_VECTOR_AT
dnl
dnl If the implementation of the C++ library provides the method
dnl std::vector::at(std::size_t), define HAVE_VECTOR_AT.
dnl
dnl @category Cxx
dnl @author Jan Langer <jan@langernetz.de>
dnl @version 2001-06-07
dnl @license AllPermissive

AC_DEFUN([AC_CXX_HAVE_VECTOR_AT],
[AC_CACHE_CHECK(whether the compiler has std::vector::at (std::size_t),
ac_cv_cxx_have_vector_at,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <vector>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[vector v (1); message.at (0); return 0;],
 ac_cv_cxx_have_vector_at=yes, ac_cv_cxx_have_vector_at=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_vector_at" = yes; then
 AC_DEFINE(HAVE_VECTOR_AT,1,[define if the compiler has the method])
else
 AC_DEFINE(HAVE_VECTOR_AT,0,[])
fi
])dnl
