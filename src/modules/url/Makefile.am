###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

SUBDIRS = caps

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviurl.la

libkviurl_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviurl_la_SOURCES = libkviurl.cpp
libkviurl_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la


noinst_HEADERS= libkviurl.h icons.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

libkviurl.cpp: libkviurl.moc
