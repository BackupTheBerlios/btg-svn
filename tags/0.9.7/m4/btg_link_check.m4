dnl 
dnl Check that the libs used to build the daemon are present.
dnl 

AC_DEFUN([BTG_LINK_CHECK],
  [
   btg_save_LIBS="$LIBS"
   btg_save_CXXFLAGS="$CXXFLAGS"
   TRY_LIBS="$btg_save_LIBS 
            -l$BOOST_IOSTREAMS_LIB -l$BOOST_FILESYSTEM_LIB -l$BOOST_DATE_TIME_LIB 
            -l$BOOST_THREAD_LIB -l$BOOST_PROGRAM_OPTIONS_LIB 
            $LIBTORRENT_LIBS $LIBGNUTLS_LIBS"
   TRY_CXXFLAGS="$btg_save_CXXFLAGS $LIBTORRENT_CFLAGS $LIBGNUTLS_CFLAGS"

   AC_MSG_NOTICE([Link test, compiler flags: $TRY_CXXFLAGS])
   AC_MSG_NOTICE([Link test, linker flags  : $TRY_LIBS])

   AC_CACHE_CHECK([if BTG can link with the provided/found libraries], btg_link_check,
     [LIBS="$TRY_LIBS"
      CXXFLAGS="$TRY_CXXFLAGS"
      AC_TRY_LINK(
        [
        // iostreams
        #include <boost/iostreams/filtering_streambuf.hpp>

        // filesystem
        #include <boost/filesystem/path.hpp>

        // date time
        #include <string>
        #include <boost/date_time/posix_time/posix_time.hpp>

        // thread
        #include <boost/thread/thread.hpp>

        // program options
        #include <boost/program_options.hpp>

        // GNU TLS
        #include <gnutls/gnutls.h>

        // libtorrent
        #include <libtorrent/session.hpp>
        ],
        [
        // iostreams
        boost::iostreams::filtering_istreambuf filter;

        // filesystem
        boost::filesystem::path bfsp("test", boost::filesystem::native);

        // date time
        boost::posix_time::ptime t(boost::posix_time::second_clock::local_time());
        std::string current_time = boost::posix_time::to_simple_string(t);

        // thread
        boost::thread thread;

        // program options
        boost::program_options::options_description desc;

        // GNU TLS
        gnutls_global_init();

        // libtorrrent
        libtorrent::session lt_session(libtorrent::fingerprint("BG", 0, 0, 0, 0));
        ],
        btg_link_check=yes, btg_link_check=no)])

   if test "$btg_link_check" = no
   then
     AC_MSG_ERROR([Unable to link with required libraries.])
   fi

   dnl restore the saved flags.
   LIBS="$btg_save_LIBS"
   CXXFLAGS="$btg_save_CXXFLAGS"
])dnl

