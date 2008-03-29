# CMakeLists for src/modules/mircimport

SET(kvimircimport_MOC_HDRS
	libkvimircimport.h
)

SET(kvimircimport_SRCS
	libkvimircimport.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimircimport_MOC_SRCS ${kvimircimport_MOC_HDRS})

# Create library
ADD_LIBRARY(kvimircimport SHARED ${kvimircimport_SRCS} ${kvimircimport_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviaddon)

# Install target
SET_TARGET_PROPERTIES(kvimircimport PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvimircimport LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)