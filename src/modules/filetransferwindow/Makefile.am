###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

SUBDIRS = caps

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvifiletransferwindow.la

libkvifiletransferwindow_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvifiletransferwindow_la_SOURCES = libkvifiletransferwindow.cpp filetransferwindow.cpp
libkvifiletransferwindow_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS= filetransferwindow.h

m_%.moc: %.h
	$(SS_QT_MOC) $< -o $@

filetransferwindow.cpp: m_filetransferwindow.moc
