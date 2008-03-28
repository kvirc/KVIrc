###############################################################################
# KVirc IRC client Maketoolbareditor - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"


pluglib_LTLIBRARIES      = libkvitoolbareditor.la

libkvitoolbareditor_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvitoolbareditor_la_SOURCES = libkvitoolbareditor.cpp toolbareditor.cpp
nodist_libkvitoolbareditor_la_SOURCES = moc_toolbareditor.cpp

noinst_HEADERS = toolbareditor.h
libkvitoolbareditor_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib4.la

moc_toolbareditor.cpp: toolbareditor.h
	$(SS_QT_MOC) $< -o $@
