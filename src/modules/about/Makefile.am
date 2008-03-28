###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviabout.la

libkviabout_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviabout_la_SOURCES = libkviabout.cpp aboutdialog.cpp

libkviabout_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS= aboutdialog.h

EXTRA_DIST = mkabouttext.pl abouttext.inc ENTRIES

abouttext:
	perl mkabouttext.pl

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

#libkviabout.cpp: libkviabout.moc
aboutdialog.cpp: aboutdialog.moc
