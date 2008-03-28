###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvilamerizer.la

libkvilamerizer_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvilamerizer_la_SOURCES = libkvilamerizer.cpp
libkvilamerizer_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib4.la

noinst_HEADERS= libkvilamerizer.h

tmpdir = $(pluglibdir)/caps/crypt/

tmp_DATA= lamerizer

EXTRA_DIST = $(tmp_DATA)


%.moc: %.h
	$(SS_QT_MOC) $< -o $@

libkvilamerizer.cpp: libkvilamerizer.moc
