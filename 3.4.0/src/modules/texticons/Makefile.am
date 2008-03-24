###############################################################################
# KVirc IRC client Makemath - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvitexticons.la

libkvitexticons_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvitexticons_la_SOURCES = libkvitexticons.cpp
libkvitexticons_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

# noinst_HEADERS=

%.moc: %.h
	$(SS_QT_MOC) $< -o $@
