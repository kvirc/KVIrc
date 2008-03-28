###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviaction.la

libkviaction_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviaction_la_SOURCES = libkviaction.cpp

libkviaction_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

#noinst_HEADERS= actiondialog.h

