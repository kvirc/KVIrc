# CMakeLists for src/modules/chan

SET(kvichan_SRCS
	libkvichan.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvichan_MOC_SRCS ${kvichan_MOC_HDRS})

# Create library
ADD_LIBRARY(kvichan SHARED ${kvichan_SRCS} ${kvichan_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvichan)

# Install target
SET_TARGET_PROPERTIES(kvichan PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvichan LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)