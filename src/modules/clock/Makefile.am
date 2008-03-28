###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviclock.la

libkviclock_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviclock_la_SOURCES = libkviclock.cpp
libkviclock_la_LIBADD  = $(SS_LIBLINK)  ../../kvilib/build/libkvirclib.la

noinst_HEADERS= libkviclock.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

libkviclock.cpp: libkviclock.moc
