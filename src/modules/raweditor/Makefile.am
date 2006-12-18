AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviraweditor.la

libkviraweditor_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviraweditor_la_SOURCES = libkviraweditor.cpp raweditor.cpp
nodist_libkviraweditor_la_SOURCES = moc_raweditor.cpp

noinst_HEADERS = raweditor.h
libkviraweditor_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

# noinst_HEADERS=

moc_raweditor.cpp: raweditor.h
	$(SS_QT_MOC) $< -o $@
