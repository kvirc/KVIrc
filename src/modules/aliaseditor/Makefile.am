###############################################################################
# KVirc IRC client Makealiaseditor - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvialiaseditor.la

libkvialiaseditor_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvialiaseditor_la_SOURCES = libkvialiaseditor.cpp aliaseditor.cpp
nodist_libkvialiaseditor_la_SOURCES = moc_aliaseditor.cpp

noinst_HEADERS = aliaseditor.h
libkvialiaseditor_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib4.la

# noinst_HEADERS=

moc_aliaseditor.cpp: aliaseditor.h
	$(SS_QT_MOC) $< -o $@
