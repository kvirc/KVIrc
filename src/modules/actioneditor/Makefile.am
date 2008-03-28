###############################################################################
# KVirc IRC client module Makefile - Szymon Stefanek <pragma at kvirc dot net>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviactioneditor.la

libkviactioneditor_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviactioneditor_la_SOURCES = libkviactioneditor.cpp actioneditor.cpp
nodist_libkviactioneditor_la_SOURCES = moc_actioneditor.cpp

noinst_HEADERS = actioneditor.h
libkviactioneditor_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

# noinst_HEADERS=

moc_actioneditor.cpp: actioneditor.h
	$(SS_QT_MOC) $< -o $@
