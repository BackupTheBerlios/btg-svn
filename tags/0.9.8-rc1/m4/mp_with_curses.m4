dnl @synopsis MP_WITH_CURSES
dnl
dnl Detect SysV compatible curses, such as ncurses.
dnl
dnl Defines HAVE_CURSES_H or HAVE_NCURSES_H if curses is found.
dnl CURSES_LIB is also set with the required libary, but is not
dnl appended to LIBS automatically. If no working curses libary is
dnl found CURSES_LIB will be left blank.
dnl
dnl This macro adds the option "--with-ncurses" to configure which can
dnl force the use of ncurses or nothing at all.
dnl
dnl @category InstalledPackages
dnl @author Mark Pulford <mark@kyne.com.au>
dnl @version 2002-04-06
dnl @license GPLWithACException

AC_DEFUN([MP_WITH_CURSES],
  [AC_ARG_WITH(ncurses, [  --with-ncurses          Force the use of ncurses over curses],,)
   mp_save_LIBS="$LIBS"
   CURSES_LIB=""
   if test "$with_ncurses" != yes
   then
     AC_CACHE_CHECK([for working curses], mp_cv_curses,
       [LIBS="$LIBS -lcurses"
        AC_TRY_LINK(
          [#include <curses.h>],
          [chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
          mp_cv_curses=yes, mp_cv_curses=no)])
     if test "$mp_cv_curses" = yes
     then
       AC_DEFINE([HAVE_CURSES_H], [], [Define if curses is installed.])
       CURSES_LIB="-lcurses"
     fi
   fi
   if test ! "$CURSES_LIB"
   then
     AC_CACHE_CHECK([for working ncurses], mp_cv_ncurses,
       [LIBS="$mp_save_LIBS -lncurses"
        AC_TRY_LINK(
          [#include <ncurses.h>],
          [chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
          mp_cv_ncurses=yes, mp_cv_ncurses=no)])
     if test "$mp_cv_ncurses" = yes
     then
       AC_DEFINE([HAVE_NCURSES_H], [], [Define if ncurses is installed.])
       CURSES_LIB="-lncurses"
     fi

     dnl Check that the ncurses being used is not the wide version.
     dnl 
     dnl 
     AC_CACHE_CHECK([for wide ncurses], mp_cv_ncursesw,
       [LIBS="$mp_save_LIBS $CURSES_LIB"
        AC_TRY_LINK(
          [#include <ncurses.h>],
          [chtype a; int b=A_STANDOUT, c=KEY_LEFT; WINDOW* w=0; cchar_t wchstr; initscr(); mvwin_wchstr(w, 0, 1, &wchstr); ],
          mp_cv_ncursesw=yes, mp_cv_ncursesw=no)])

     if test "$mp_cv_ncursesw" = yes
     then
       AC_MSG_ERROR([Attempt to use ncurses with wide character support.])
     fi

   fi
   LIBS="$mp_save_LIBS"
])dnl
