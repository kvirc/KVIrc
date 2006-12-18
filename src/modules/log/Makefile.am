###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvilog.la

libkvilog_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvilog_la_SOURCES = libkvilog.cpp
libkvilog_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

# noinst_HEADERS=

%.moc: %.h
	$(SS_QT_MOC) $< -o $@
