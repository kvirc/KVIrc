TARGET = kvimy
 
HEADERS += idle.h
 						
SOURCES += libkvimy.cpp idle_x11.cpp idle_mac.cpp idle.cpp
 						
include(../module.pri)