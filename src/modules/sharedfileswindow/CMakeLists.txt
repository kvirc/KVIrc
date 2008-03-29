# CMakeLists for src/modules/sharedfilewindow

SET(kvieditor_MOC_HDRS
	sharedfileswindow.h
)

SET(kvisharedfilewindow_SRCS
	libkvisharedfileswindow.cpp
	sharedfileswindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisharedfilewindow_MOC_SRCS ${kvisharedfilewindow_MOC_HDRS})

# Create library
ADD_LIBRARY(kvisharedfilewindow SHARED ${kvisharedfilewindow_SRCS} ${kvisharedfilewindow_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvisharedfilewindow)

# Install target
SET_TARGET_PROPERTIES(kvisharedfilewindow PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvisharedfilewindow LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)