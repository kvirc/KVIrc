###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

SUBDIRS = caps

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvisharedfileswindow.la

libkvisharedfileswindow_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvisharedfileswindow_la_SOURCES = libkvisharedfileswindow.cpp sharedfileswindow.cpp
libkvisharedfileswindow_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib4.la

noinst_HEADERS= sharedfileswindow.h

m_%.moc: %.h
	$(SS_QT_MOC) $< -o $@

sharedfileswindow.cpp: m_sharedfileswindow.moc
