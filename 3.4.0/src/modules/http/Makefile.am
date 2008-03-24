###############################################################################
# KVirc IRC client Makefile - 31.01.2002 Szymon Stefanek <stefanek@tin.it>
###############################################################################

tmpdir=$(picsdir)

tmp_DATA=kvi_httpicons.png

EXTRA_DIST=kvi_httpicons.png

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvihttp.la

libkvihttp_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvihttp_la_SOURCES = libkvihttp.cpp httpfiletransfer.cpp
libkvihttp_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS= httpfiletransfer.h

m_%.moc: %.h
	$(SS_QT_MOC) $< -o $@

httpfiletransfer.cpp: m_httpfiletransfer.moc
