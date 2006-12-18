###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvitip.la

libkvitip_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvitip_la_SOURCES = libkvitip.cpp
libkvitip_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS= libkvitip.h

tmpdir = $(picsdir)

tmp_DATA = kvi_tip.png

confdir = $(configmodulesdir)

conf_DATA = libkvitip.kvc libkvitip_it.kvc libkvitip_cs.kvc libkvitip_ru.kvc

EXTRA_DIST = $(tmp_DATA) $(conf_DATA)

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

libkvitip.cpp: libkvitip.moc
