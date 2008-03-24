###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvihelp.la

libkvihelp_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvihelp_la_SOURCES = libkvihelp.cpp helpwidget.cpp helpwindow.cpp index.cpp index.moc
libkvihelp_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS= helpwidget.h helpwindow.h index.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

helpwidget.cpp: helpwidget.moc
helpwindow.cpp: helpwindow.moc
index.cpp: index.moc
