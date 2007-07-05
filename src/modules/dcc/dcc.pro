TARGET = kvidcc
 
HEADERS += adpcmcodec.h \
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
 						
SOURCES += adpcmcodec.cpp \
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
 						
include(../module.pri)

LIBS += -lws2_32