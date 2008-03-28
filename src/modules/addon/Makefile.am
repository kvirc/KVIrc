###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

SUBDIRS = pics

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviaddon.la

libkviaddon_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviaddon_la_SOURCES = libkviaddon.cpp managementdialog.cpp
nodist_libkviaddon_la_SOURCES = moc_managementdialog.cpp

libkviaddon_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib4.la

noinst_HEADERS= managementdialog.h

moc_managementdialog.cpp: managementdialog.h
	$(SS_QT_MOC) $< -o $@
