INCLUDEPATH += ../kvilib/tal/ \
    ../kvilib/config/ \
    ../kvilib/core/ \
    ../kvilib/ext/ \
    ../kvilib/file/ \
    ../kvilib/irc/ \
    ../kvilib/net/ \
    ../kvilib/system/
    
!contains( DEFINES, __KVILIB__ ) {
      LIBS += -lkvilib
  } 