# CMakeLists for src/modules/editor

SET(kvieditor_MOC_HDRS
	scripteditor.h
)

SET(kvieditor_SRCS
	libkvieditor.cpp
	scripteditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvieditor_MOC_SRCS ${kvieditor_MOC_HDRS})

# Create library
ADD_LIBRARY(kvieditor SHARED ${kvieditor_SRCS} ${kvieditor_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvieditor)

# Install target
SET_TARGET_PROPERTIES(kvieditor PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvieditor LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)