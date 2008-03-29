# CMakeLists for src/modules/snd

# From qmake
#win32{
#	LIBS += -lwinmm
#}

SET(kvieditor_MOC_HDRS
	libkvisnd.h
)

SET(kvisnd_SRCS
	libkvisnd.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisnd_MOC_SRCS ${kvisnd_MOC_HDRS})

# Create library
ADD_LIBRARY(kvisnd SHARED ${kvisnd_SRCS} ${kvisnd_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvisnd)

# Install target
SET_TARGET_PROPERTIES(kvisnd PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvisnd LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)