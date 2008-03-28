###############################################################################
# KVirc IRC client Makefile - 3.23.2002 Juanjo Álvarez <juanjux@yahoo.es>
###############################################################################

SUBDIRS = caps

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvilogview.la

libkvilogview_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvilogview_la_SOURCES = libkvilogview.cpp logviewwidget.cpp logviewmdiwindow.cpp
libkvilogview_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib4.la

noinst_HEADERS= logviewwidget.h logviewmdiwindow.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

logviewwidget.cpp: logviewwidget.moc
logviewmdiwindow.cpp: logviewmdiwindow.moc
