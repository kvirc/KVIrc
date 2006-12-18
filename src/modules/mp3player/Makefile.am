###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvimp3player.la
METASOURCES = AUTO

libkvimp3player_la_LDFLAGS = -avoid-version -module

libkvimp3player_la_SOURCES = libkvimp3player.cpp
libkvimp3player_la_LIBADD = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la


INCLUDES = -I$(top_srcdir)/src/modules/mp3player

