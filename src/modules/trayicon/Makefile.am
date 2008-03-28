###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvitrayicon.la

libkvitrayicon_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvitrayicon_la_SOURCES = libkvitrayicon.cpp

#nodist_libkvidockwidget_la_SOURCES = moc_trayicon.cpp

libkvitrayicon_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS= libkvitrayicon.h

tmpdir = $(picsdir)

tmp_DATA = kvi_dock.png kvi_dock_part-0.png kvi_dock_part-1.png kvi_dock_part-2.png
   
EXTRA_DIST = $(tmp_DATA)

%.moc: %.h
	$(SS_QT_MOC) $< -o $@
	
moc_%.cpp: %.h
	$(SS_QT_MOC) $< -o $@

libkvitrayicon.cpp: libkvitrayicon.moc
