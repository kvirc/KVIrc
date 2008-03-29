# CMakeLists for src/modules/perl

SET(kviperl_SRCS
	libkviperl.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviperl_MOC_SRCS ${kviperl_MOC_HDRS})

# Create library
ADD_LIBRARY(kviperl SHARED ${kviperl_SRCS} ${kviperl_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviaddon)

# Install target
SET_TARGET_PROPERTIES(kviperl PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviperl LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)