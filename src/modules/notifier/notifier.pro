TARGET = kvinotifier
 
HEADERS += \
			notifiersettings.h \
			notifierwindow.h \
			notifiermessage.h \
			notifierwindowtabs.h \
			notifierwindowbody.h \
			notifierwindowborder.h \
			notifierwindowprogressbar.h 
 						
SOURCES += libkvinotifier.cpp \
							notifierwindow.cpp \
							notifiermessage.cpp \
							notifierwindowtabs.cpp \
							notifierwindowbody.cpp \
							notifierwindowborder.cpp \
							notifierwindowprogressbar.cpp
 						
include(../module.pri)