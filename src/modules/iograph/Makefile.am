###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviiograph.la

libkviiograph_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviiograph_la_SOURCES = libkviiograph.cpp
libkviiograph_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS= libkviiograph.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

libkviiograph.cpp: libkviiograph.moc
