###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvilist.la

libkvilist_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvilist_la_SOURCES = libkvilist.cpp listwindow.cpp
libkvilist_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS= listwindow.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

listwindow.cpp: listwindow.moc
