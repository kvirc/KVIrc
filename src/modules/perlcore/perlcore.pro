TARGET = kviperlcore

INCLUDEPATH += $(PERLDIR)/lib/CORE/

LIBS += -lperl58
 
HEADERS += perlcoreinterface.h
 						
SOURCES += libkviperlcore.cpp
 						
include(../module.pri)