###############################################################################
# KVirc IRC client Makefile - 31.01.2002 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvichan.la

libkvichan_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvichan_la_SOURCES = libkvichan.cpp
libkvichan_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

# noinst_HEADERS=

%.moc: %.h
	$(SS_QT_MOC) $< -o $@
