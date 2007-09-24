bin_PROGRAMS = btgvs

btgvs_SOURCES = arg.cpp handler.cpp ui.cpp btgvs.cpp

noinst_HEADERS = arg.h handler.h ui.h btgvs.h

btgvs_LDADD   = $(top_builddir)/bcore/libbtgcore.la \
                $(top_builddir)/bcore/client/libbtgclient.la \
                $(top_builddir)/external/ini/libini.la

btgvs_LDFLAGS  = @LIBS@ -l@BOOST_DATE_TIME_LIB@ -l@BOOST_IOSTREAMS_LIB@ -l@BOOST_FILESYSTEM_LIB@ @LIBGNUTLS_LIBS@ -l@BOOST_PROGRAM_OPTIONS_LIB@ -l@BOOST_THREAD_LIB@ `agar-config --libs`

# @SDL_LIBS@ -lSDL_ttf -lSDL_gfx

btgvs_CXXFLAGS = @CXXFLAGS@ @LIBGNUTLS_CFLAGS@ `agar-config --cflags` \
                 -DBTG_DEBUG=@BTG_DEBUG@ -I`agar-config --incldir`/agar

btgvs_CFLAGS   = @CFLAGS@ @LIBGNUTLS_CFLAGS@ `agar-config --cflags` \
                 -DBTG_DEBUG=@BTG_DEBUG@ -I`agar-config --incldir`/agar

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/external
#man1_MANS  = btgui.man
#EXTRA_DIST = btgui.man
