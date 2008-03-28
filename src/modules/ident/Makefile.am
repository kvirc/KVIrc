###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviident.la

libkviident_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviident_la_SOURCES = libkviident.cpp
libkviident_la_LIBADD  = $(SS_LIBLINK)  ../../kvilib/build/libkvilib4.la

noinst_HEADERS= libkviident.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

libkviident.cpp: libkviident.moc
