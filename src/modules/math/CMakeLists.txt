# CMakeLists for src/modules/math

SET(kvimath_SRCS
	libkvimath.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimath_MOC_SRCS ${kvimath_MOC_HDRS})

# Create library
ADD_LIBRARY(kvimath SHARED ${kvimath_SRCS} ${kvimath_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvimath)

# Install target
SET_TARGET_PROPERTIES(kvimath PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvimath LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)