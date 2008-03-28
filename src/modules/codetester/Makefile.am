AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvicodetester.la

libkvicodetester_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvicodetester_la_SOURCES = libkvicodetester.cpp codetester.cpp
nodist_libkvicodetester_la_SOURCES = moc_codetester.cpp

noinst_HEADERS = codetester.h
libkvicodetester_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib4.la

# noinst_HEADERS=

moc_codetester.cpp: codetester.h
	$(SS_QT_MOC) $< -o $@
