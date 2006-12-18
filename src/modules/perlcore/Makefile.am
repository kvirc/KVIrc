###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\" $(SS_PERL_CCFLAGS)

pluglib_LTLIBRARIES      = libkviperlcore.la

libkviperlcore_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS) $(SS_PERL_LDFLAGS)

libkviperlcore_la_SOURCES = libkviperlcore.cpp
libkviperlcore_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS = perlcoreinterface.h

EXTRA_DIST = KVIrc.xs ppport.h xs.inc typemap

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

xs:
	xsubpp -noversioncheck -C++ -noprototypes KVIrc.xs > xs.inc
