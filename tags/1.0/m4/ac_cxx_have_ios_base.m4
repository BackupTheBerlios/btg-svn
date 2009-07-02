AC_DEFUN([AC_CXX_HAVE_IOS_BASE],
[AC_CACHE_CHECK(whether the compiler has ios_base,
ac_cv_cxx_have_ios_base,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
#if HAVE_FSTREAM
#  include <fstream>
#elif HAVE_FSTREAM_H
#  include <fstream.h>
#else
#error "No <fstream> or <fstream.h>"
#endif
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[ofstream file; file.open("/tmp/test.file", ios_base::out); return 0;],
 ac_cv_cxx_have_ios_base=yes, ac_cv_cxx_have_ios_base=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_ios_base" = yes; then
 AC_DEFINE(HAVE_IOS_BASE,1,[define if the compiler can use ios_base])
else
 AC_DEFINE(HAVE_IOS_BASE,0,[define if the compiler can use ios_base])
fi
])
