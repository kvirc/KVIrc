# CMakeLists for src/modules/tmphighlight

SET(kvitmphighlight_SRCS
	libkvitmphighlight.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitmphighlight_MOC_SRCS ${kvitmphighlight_MOC_HDRS})

# Create library
ADD_LIBRARY(kvitmphighlight SHARED ${kvitmphighlight_SRCS} ${kvitmphighlight_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvitmphighlight)

# Install target
SET_TARGET_PROPERTIES(kvitmphighlight PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvitmphighlight LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)