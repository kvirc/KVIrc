###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvitorrent.la

#%.moc: %.h
#	$(SS_QT_MOC) $< -o $@

libkvitorrent_la_LDFLAGS = -avoid-version -module

libkvitorrent_la_SOURCES = libkvitorrent.cpp \
		tc_interface.cpp \
		tc_ktorrentdcopinterface.cpp \
		tc_statusbarapplet.cpp
		
libkvitorrent_la_LIBADD = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS = tc_interface.h \
		tc_ktorrentdcopinterface.h \
		tc_statusbarapplet.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

tc_ktorrentdcopinterface.cpp: tc_ktorrentdcopinterface.moc
tc_statusbarapplet.cpp: tc_statusbarapplet.moc
