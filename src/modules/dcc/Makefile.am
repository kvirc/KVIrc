###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

tmpdir=$(picsdir)

tmp_DATA=kvi_dccfiletransfericons.png

EXTRA_DIST=kvi_dccfiletransfericons.png


AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkvidcc.la

libkvidcc_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvidcc_la_SOURCES = adpcmcodec.cpp \
										broker.cpp \
										canvas.cpp \
										canvaswidget.cpp \
										chat.cpp \
										codec.cpp \
										descriptor.cpp \
										dialogs.cpp \
										gsmcodec.cpp \
										libkvidcc.cpp \
										marshal.cpp \
										requests.cpp \
										send.cpp \
										thread.cpp \
										utils.cpp \
										voice.cpp \
										window.cpp

libkvidcc_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS= adpcmcodec.h \
								broker.h \
								canvas.h \
								canvaswidget.h \
								chat.h \
								codec.h \
								descriptor.h \
								dialogs.h \
								gsmcodec.h \
								marshal.h \
								send.h \
								thread.h \
								utils.h \
								voice.h \
								window.h

m_%.moc: %.h
	$(SS_QT_MOC) $< -o $@

broker.cpp: m_broker.moc
canvas.cpp: m_canvas.moc
canvaswidget.cpp: m_canvaswidget.moc
chat.cpp: m_chat.moc
dialogs.cpp: m_dialogs.moc
marshal.cpp: m_marshal.moc
send.cpp: m_send.moc
voice.cpp: m_voice.moc
window.cpp: m_window.moc
