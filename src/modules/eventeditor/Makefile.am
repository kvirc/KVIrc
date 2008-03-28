###############################################################################
# KVirc IRC client Makeeventeditor - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvieventeditor.la

libkvieventeditor_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvieventeditor_la_SOURCES = libkvieventeditor.cpp eventeditor.cpp
nodist_libkvieventeditor_la_SOURCES = moc_eventeditor.cpp

noinst_HEADERS = eventeditor.h
libkvieventeditor_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

# noinst_HEADERS=

moc_eventeditor.cpp: eventeditor.h
	$(SS_QT_MOC) $< -o $@
