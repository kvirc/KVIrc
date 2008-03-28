###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

SUBDIRS = pics

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvinotifier.la

libkvinotifier_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvinotifier_la_SOURCES = libkvinotifier.cpp \
							notifierwindow.cpp \
							notifiermessage.cpp \
							notifierwindowtabs.cpp \
							notifierwindowbody.cpp \
							notifierwindowborder.cpp \
							notifierwindowprogressbar.cpp
							
libkvinotifier_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS= \
			notifiersettings.h \
			notifierwindow.h \
			notifiermessage.h \
			notifierwindowtabs.h \
			notifierwindowbody.h \
			notifierwindowborder.h \
			notifierwindowprogressbar.h

confdir = $(configmodulesdir)

conf_DATA = libkvinotifier.kvc

EXTRA_DIST = $(conf_DATA)

m_%.moc: %.h
	$(SS_QT_MOC) $< -o $@

notifierwindow.cpp: m_notifierwindow.moc
notifierwindowtabs.cpp: m_notifierwindowtabs.moc

