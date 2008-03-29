# CMakeLists for src/modules/toolbareditor

SET(kvieventeditor_MOC_HDRS
	toolbareditor.h
)

SET(kvitoolbareditor_SRCS
	libkvitoolbareditor.cpp
	toolbareditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitoolbareditor_MOC_SRCS ${kvitoolbareditor_MOC_HDRS})

# Create library
ADD_LIBRARY(kvitoolbareditor SHARED ${kvitoolbareditor_SRCS} ${kvitoolbareditor_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvitoolbareditor)

# Install target
SET_TARGET_PROPERTIES(kvitoolbareditor PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvitoolbareditor LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)