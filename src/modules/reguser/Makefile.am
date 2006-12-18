###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvireguser.la

libkvireguser_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvireguser_la_SOURCES = libkvireguser.cpp edituser.cpp wizard.cpp
libkvireguser_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS= edituser.h wizard.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

edituser.cpp: edituser.moc
wizard.cpp: wizard.moc
