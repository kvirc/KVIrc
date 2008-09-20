###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvimediaplayer.la

#%.moc: %.h
#	$(SS_QT_MOC) $< -o $@

libkvimediaplayer_la_LDFLAGS = -avoid-version -module $(SS_LIBDIRS)

libkvimediaplayer_la_SOURCES = libkvimediaplayer.cpp \
		mp_amarokinterface.cpp \
		mp_amipinterface.cpp \
		mp_dcopinterface.cpp \
		mp_interface.cpp \
		mp_jukinterface.cpp \
		mp_mp3.cpp \
		mp_winampinterface.cpp \
		mp_xmmsinterface.cpp
		
libkvimediaplayer_la_LIBADD = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS = mp_amarokinterface.h \
		mp_amipinterface.h \
		mp_dcopinterface.h \
		mp_interface.h \
		mp_jukinterface.h \
		mp_mp3.h \
		mp_winampinterface.h \
		mp_xmmsinterface.h


