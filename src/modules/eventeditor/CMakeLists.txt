# CMakeLists for src/modules/eventeditor

SET(kvieventeditor_MOC_HDRS
	eventeditor.h
)

SET(kvieventeditor_SRCS
	eventeditor.cpp
	libkvieventeditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvieventeditor_MOC_SRCS ${kvieventeditor_MOC_HDRS})

# Create library
ADD_LIBRARY(kvieventeditor SHARED ${kvieventeditor_SRCS} ${kvieventeditor_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvieventeditor)

# Install target
SET_TARGET_PROPERTIES(kvieventeditor PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvieventeditor LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)