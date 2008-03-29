# CMakeLists for src/modules/spaste

SET(kvieventeditor_MOC_HDRS
	controller.h
	libkvispaste.h
)

SET(kvispaste_SRCS
	libkvispaste.cpp
	controller.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvispaste_MOC_SRCS ${kvispaste_MOC_HDRS})

# Create library
ADD_LIBRARY(kvispaste SHARED ${kvispaste_SRCS} ${kvispaste_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvispaste)

# Install target
SET_TARGET_PROPERTIES(kvispaste PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvispaste LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)