CONFIG   += precompile_header debug_and_release thread dll
LANGUAGE  = C++
TEMPLATE = lib

QT += core \
    gui \
    qt3support
    
DEFINES -= UNICODE

CONFIG(debug, debug|release) {
     DESTDIR = ../../../bin/debug/modules/
     LIBS += -L../../../bin/debug/
 } else {
     DESTDIR = ../../../bin/release/modules/
     LIBS += -L../../../bin/release/
 }
    
INCLUDEPATH += ../../kvilib/tal/ \
    ../../kvilib/config/ \
    ../../kvilib/core/ \
    ../../kvilib/ext/ \
    ../../kvilib/file/ \
    ../../kvilib/irc/ \
    ../../kvilib/net/ \
    ../../kvilib/system/
          
  
INCLUDEPATH += ../../kvirc/kernel/ \
    ../../kvirc/kvs/ \
    ../../kvirc/module/ \
    ../../kvirc/sparser/ \
    ../../kvirc/ui/
    
LIBS += -lkvilib \
      -lkvirc
      
RC_FILE = ../../../data/resources/commctrl.rc