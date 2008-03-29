# CMakeLists for src/modules/torrent

SET(kvieventeditor_MOC_HDRS
	tc_statusbarapplet.h
	tc_ktorrentdcopinterface.h
	tc_interface.h
)

SET(kvitorrent_SRCS
	libkvitorrent.cpp
	tc_statusbarapplet.cpp
	tc_ktorrentdcopinterface.cpp
	tc_interface.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitorrent_MOC_SRCS ${kvitorrent_MOC_HDRS})

# Create library
ADD_LIBRARY(kvitorrent SHARED ${kvitorrent_SRCS} ${kvitorrent_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvitorrent)

# Install target
SET_TARGET_PROPERTIES(kvitorrent PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvitorrent LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)