pre_check:
	@echo "Creating files used for tests."
	@rm -fr btg_utest
	@mkdir btg_utest && echo "Created test directory"
	@touch btg_utest/test.mque
	@touch btg_utest/btgdaemon
	@touch btg_utest/shmem_file
	@touch btg_utest/file0 btg_utest/file1 
	@touch btg_utest/file2 btg_utest/file3
	@touch btg_utest/file4 btg_utest/file5
	@touch btg_utest/file6 btg_utest/file7
	@touch btg_utest/file8 btg_utest/file9
	@touch btg_utest/file10 btg_utest/file11 
	@touch btg_utest/test.daemon.config
	@touch btg_utest/test.daemon.iplist
	@mkdir btg_utest/finished
	@mkdir btg_utest/fileop
	@mkdir btg_utest/dir1
	@touch btg_utest/file1

	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX0" > btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX1" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX2" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX3" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX4" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX5" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX6" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX7" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX8" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX9" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX10" >> btg_utest/file.torrent
	@echo "xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX11" >> btg_utest/file.torrent

gen_certs:
	@mkdir btg_utest
	@certtool --generate-privkey --outfile btg_utest/daemon.pem

post_check:
	@echo "Removing files used for tests."
	@rm -fr btg_utest

check: pre_check $(TESTS) 
	@echo "Running tests done."
	$(MAKE) post_check

repeat_test: pre_check
	./testrunner 150
	$(MAKE) post_check

TESTS = testrunner

if BUILDGUI
  BCORE_CLIENT_TEST        = test_bcore_client.cpp
  BCORE_CLIENT_TEST_HEADER = test_bcore_client.h
  BCORE_CLIENT_TEST_LIB    = $(top_builddir)/bcore/client/libbtgclient.la
endif

if BUILDCLI
  BCORE_CLIENT_TEST        = test_bcore_client.cpp
  BCORE_CLIENT_TEST_HEADER = test_bcore_client.h
  BCORE_CLIENT_TEST_LIB    = $(top_builddir)/bcore/client/libbtgclient.la
endif

if BUILDURL
  URL_DEFINES=-DBTG_OPTION_URL=@BTG_OPTION_URL@ @LIBCURL_CPPFLAGS@
  URL_LIBS=@LIBCURL@
else
  URL_DEFINES=-DBTG_OPTION_URL=@BTG_OPTION_URL@
endif

testrunner_SOURCES = listener.cpp testrunner.cpp test_daemon.cpp test_os.cpp test_gen_transport.cpp test_tcpip.cpp test_http.cpp test_bcore.cpp $(top_srcdir)/bcore/configuration.cpp $(top_srcdir)/bcore/os/gnutlsif.cpp $(BCORE_CLIENT_TEST)

noinst_HEADERS     = listener.h testrunner.h test_daemon.h test_os.h test_gen_transport.h test_tcpip.h test_http.h test_bcore.h files.h $(top_srcdir)/bcore/configuration.h $(BCORE_CLIENT_TEST_HEADER)

noinst_PROGRAMS     = testrunner

testrunner_LDADD   = $(top_builddir)/bcore/libbtgcore.la $(BCORE_CLIENT_TEST_LIB) \
                     $(top_builddir)/daemon/libdaemon.la \
                     $(top_builddir)/daemon/http/libhttp.la \
                     $(top_builddir)/daemon/resource/libresource.la \
                     $(top_builddir)/external/ini/libini.la \
                     $(top_builddir)/bcore/libbtgcore.la

testrunner_LDFLAGS  = @LIBS@ $(CPPUNIT_LIBS) @LIBTORRENT_LIBS@ -l@BOOST_IOSTREAMS_LIB@ -l@BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ $(URL_LIBS)

testrunner_CFLAGS   = @CFLAGS@   $(CPPUNIT_CFLAGS) @LIBTORRENT_CFLAGS@ @LIBGNUTLS_CFLAGS@ \
                      -DBTG_DEBUG=@BTG_DEBUG@ \
                      -DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
                      -DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
                      -DBTG_REV="@BTG_REV@" $(URL_DEFINES)

testrunner_CXXFLAGS = @CXXFLAGS@ $(CPPUNIT_CFLAGS) @LIBTORRENT_CFLAGS@ @LIBGNUTLS_CFLAGS@ \
                      -DBTG_DEBUG=@BTG_DEBUG@ \
                      -DBTG_OPTION_SAVESESSIONS=@BTG_OPTION_SAVESESSIONS@ \
                      -DBTG_OPTION_EVENTCALLBACK=@BTG_OPTION_EVENTCALLBACK@ \
                      -DBTG_REV="@BTG_REV@" $(URL_DEFINES)

AM_CFLAGS         = @CFLAGS@   @DEBUG_CFLAGS@ \
                    -DBTG_DEBUG=@BTG_DEBUG@ \
                    -DBTG_VERSION="@VERSION@" \
                    -DBTG_BUILD="@BTG_BUILD@" \
                    -DBTG_REV="@BTG_REV@" $(URL_DEFINES)

AM_CXXFLAGS       = @CXXFLAGS@ @DEBUG_CXXFLAGS@ \
                    -DBTG_DEBUG=@BTG_DEBUG@ \
                    -DBTG_VERSION="@VERSION@" \
                    -DBTG_BUILD="@BTG_BUILD@" \
                    -DBTG_REV="@BTG_REV@" $(URL_DEFINES)

INCLUDES = -I$(top_srcdir)
