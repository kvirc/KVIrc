###############################################################################
# KVirc IRC client Makefile - 30.12.2006 Szymon Stefanek <pragma at kvirc dot net>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvitheme.la

libkvitheme_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvitheme_la_SOURCES = libkvitheme.cpp \
		managementdialog.cpp \
		packthemedialog.cpp \
		savethemedialog.cpp \
		themefunctions.cpp

nodist_libkvitheme_la_SOURCES = moc_managementdialog.cpp \
		moc_packthemedialog.cpp \
		moc_savethemedialog.cpp

libkvitheme_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS= managementdialog.h \
		packthemedialog.h \
		savethemedialog.h \
		themefunctions.h

moc_managementdialog.cpp: managementdialog.h
	$(SS_QT_MOC) $< -o $@

moc_packthemedialog.cpp: packthemedialog.h
	$(SS_QT_MOC) $< -o $@
	
moc_savethemedialog.cpp: savethemedialog.h
	$(SS_QT_MOC) $< -o $@
