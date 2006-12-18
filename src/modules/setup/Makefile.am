###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvisetup.la

libkvisetup_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvisetup_la_SOURCES = libkvisetup.cpp setupwizard.cpp
libkvisetup_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS= setupwizard.h

#datadir = $(picsdir)

#data_DATA = kvi_dock.png
   
#EXTRA_DIST = $(data_DATA)

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

setupwizard.cpp: setupwizard.moc
