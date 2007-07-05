TARGET = kvisetup
 
HEADERS += setupwizard.h
 						
SOURCES += libkvisetup.cpp setupwizard.cpp
 						
LIBS = -lole32 -luuid

include(../module.pri)