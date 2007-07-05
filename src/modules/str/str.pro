TARGET = kvistr
 
SOURCES += libkvistr.cpp

LIBS += -leay32 \
    -lssleay32 
 						
include(../module.pri)