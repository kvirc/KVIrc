INCLUDEPATH += kernel/ \
    kvs/ \
    module/ \
    sparser/ \
    ui/
    
!contains( DEFINES, __KVIRC__ ) {
      LIBS += -lkvirc
  } 