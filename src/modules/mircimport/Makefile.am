###############################################################################
# KVirc IRC client Makefile- 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

SUBDIRS = caps pics

pluglib_LTLIBRARIES      = libkvimircimport.la

libkvimircimport_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvimircimport_la_SOURCES = libkvimircimport.cpp
libkvimircimport_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS= libkvimircimport.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

libkvimircimport.cpp: libkvimircimport.moc
