# CMakeLists for src/modules/http

SET(kvihttp_MOC_HDRS
	httpfiletransfer.h
)

SET(kvihttp_SRCS
	libkvihttp.cpp
	httpfiletransfer.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvihttp_MOC_SRCS ${kvihttp_MOC_HDRS})

# Create library
ADD_LIBRARY(kvihttp SHARED ${kvihttp_SRCS} ${kvihttp_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvihttp)

# Install target
SET_TARGET_PROPERTIES(kvihttp PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvihttp LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)