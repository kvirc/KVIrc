CONFIG   += precompile_header debug_and_release thread
LANGUAGE  = C++
TEMPLATE = app
TARGET = win32registrar

DEFINES += _UNICODE

CONFIG(debug, debug|release) {
     DESTDIR = ../../bin/debug/
 } else {
     DESTDIR = ../../bin/release/
 }
 
HEADERS += win32registrar.h
    
    
SOURCES += win32registrar.cpp

LIBS += -lshlwapi

RC_FILE = win32registrar.rc