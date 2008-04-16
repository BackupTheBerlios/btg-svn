AC_DEFUN([AC_CXX_HAVE_STRING_CLEAR],
[AC_CACHE_CHECK(whether the compiler has std::string::clear(),
ac_cv_cxx_have_string_clear,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <string>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[string message = "test"; message.clear(); return 0;],
 ac_cv_cxx_have_string_clear=yes, ac_cv_cxx_have_string_clear=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_string_clear" = yes; then
 AC_DEFINE(HAVE_STRING_CLEAR,1,[define if the compiler has the method std::string::clear()])
else
 AC_DEFINE(HAVE_STRING_CLEAR,0,[define if the compiler has the method std::string::clear()])
fi
])dnl
