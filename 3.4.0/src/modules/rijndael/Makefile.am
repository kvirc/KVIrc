###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

SUBDIRS = caps

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvirijndael.la

libkvirijndael_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvirijndael_la_SOURCES = libkvirijndael.cpp rijndael.cpp ablowfish.cpp
libkvirijndael_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS= libkvirijndael.h rijndael.h ablowfish.h


%.moc: %.h
	$(SS_QT_MOC) $< -o $@

libkvirijndael.cpp: libkvirijndael.moc
