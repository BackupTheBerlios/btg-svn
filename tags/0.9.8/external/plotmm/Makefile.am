if BUILDINTERNALPLOTMM

pkglib_LTLIBRARIES = libplotmm.la

libplotmm_la_SOURCES = \
	paint.h paint.cc \
	rectangle.h \
	doublerect.h doublerect.cc \
	symbol.h symbol.cc \
	errorcurve.h errorcurve.cc \
	curve.h curve.cc \
	doubleintmap.h doubleintmap.cc \
	scale.h scale.cc \
	scalediv.h scalediv.cc \
	supplemental.h supplemental.cc \
	plot.h plot.cc \
	compat.h

#libplotmm_la_CXXFLAGS = @PACKAGE_CFLAGS@ -I$(top_srcdir)/external
libplotmm_la_CXXFLAGS = @GTKMM_CFLAGS@ -I$(top_srcdir)/external

# CLEANFILES = *~

endif

