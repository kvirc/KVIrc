# CMakeLists for src/modules/lamerizer

SET(kvilamerizer_MOC_HDRS
	libkvilamerizer.h
)

SET(kvilamerizer_SRCS
	libkvilamerizer.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilamerizer_MOC_SRCS ${kvilamerizer_MOC_HDRS})

# Create library
ADD_LIBRARY(kvilamerizer SHARED ${kvilamerizer_SRCS} ${kvilamerizer_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvilamerizer)

# Install target
SET_TARGET_PROPERTIES(kvilamerizer PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvilamerizer LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)