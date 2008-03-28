AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvipopupeditor.la

libkvipopupeditor_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvipopupeditor_la_SOURCES = libkvipopupeditor.cpp popupeditor.cpp
nodist_libkvipopupeditor_la_SOURCES = moc_popupeditor.cpp

noinst_HEADERS = popupeditor.h
libkvipopupeditor_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

# noinst_HEADERS=

moc_popupeditor.cpp: popupeditor.h
	$(SS_QT_MOC) $< -o $@
