###############################################################################
# KVirc IRC client Makestr - 10.03.2002 Juanjo Álvarez <juanjux@yahoo.es>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
-I/usr/include/kde/arts\
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvisnd.la

libkvisnd_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvisnd_la_SOURCES = libkvisnd.cpp
libkvisnd_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la


noinst_HEADERS= libkvisnd.h

%.moc: %.h
	$(SS_QT_MOC) $< -o $@

libkvisnd.cpp: libkvisnd.moc
