# CMakeLists for src/modules/filetransferwindow

SET(kvifiletransferwindow_MOC_HDRS
	filetransferwindow.h
)

SET(kvifiletransferwindow_SRCS
	libkvifiletransferwindow.cpp
	filetransferwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvifiletransferwindow_MOC_SRCS ${kvifiletransferwindow_MOC_HDRS})

# Create library
ADD_LIBRARY(kvifiletransferwindow SHARED ${kvifiletransferwindow_SRCS} ${kvifiletransferwindow_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvifiletransferwindow)

# Install target
SET_TARGET_PROPERTIES(kvifiletransferwindow PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvifiletransferwindow LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)