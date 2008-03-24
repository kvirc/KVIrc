###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvidockwidget.la

libkvidockwidget_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvidockwidget_la_SOURCES = libkvidockwidget_qt3.cpp libkvidockwidget_qt4.cpp

#nodist_libkvidockwidget_la_SOURCES = moc_trayicon.cpp

libkvidockwidget_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS= libkvidockwidget.h libkvidockwidget_qt4.h libkvidockwidget_qt3.h

tmpdir = $(picsdir)

tmp_DATA = kvi_dock.png kvi_dock_part-0.png kvi_dock_part-1.png kvi_dock_part-2.png
   
EXTRA_DIST = $(tmp_DATA)

%.moc: %.h
	$(SS_QT_MOC) $< -o $@
	
moc_%.cpp: %.h
	$(SS_QT_MOC) $< -o $@

libkvidockwidget_qt3.cpp: libkvidockwidget_qt3.moc
libkvidockwidget_qt4.cpp: libkvidockwidget_qt4.moc
