###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\" -DKVIRC_SSEX

pluglib_LTLIBRARIES      = libkvieditor.la

libkvieditor_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

nodist_libkvieditor_la_SOURCES = moc_scripteditor.cpp
libkvieditor_la_SOURCES = libkvieditor.cpp scripteditor.cpp
libkvieditor_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS= scripteditor.h

moc_%.cpp: %.h
	$(SS_QT_MOC) $< -o $@

