###############################################################################
# KVirc IRC client Makestr - 10.03.2002 Juanjo Álvarez <juanjux@yahoo.es>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
-I/usr/include/kde/arts\
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

#ARTS_LD_FLAGS = -DPIC -fPIC 
#ARTS_C_FLAGS = -L/usr/lib -ldl -lartsc -lpthread
pluglib_LTLIBRARIES      = libkvitmphighlight.la

libkvitmphighlight_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvitmphighlight_la_SOURCES = libkvitmphighlight.cpp
libkvitmphighlight_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib4.la


#noinst_HEADERS= 

%.moc: %.h
	$(SS_QT_MOC) $< -o $@
