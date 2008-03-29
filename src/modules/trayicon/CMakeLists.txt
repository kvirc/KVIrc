# CMakeLists for src/modules/trayicon

SET(kvieventeditor_MOC_HDRS
	libkvitrayicon.h
	trayicon.h
)

SET(kvitrayicon_SRCS
	libkvitrayicon.cpp
	trayicon.cpp
	trayicon_x11.cpp
	trayicon_mac.cpp
	trayicon_win.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitrayicon_MOC_SRCS ${kvitrayicon_MOC_HDRS})

# Create library
ADD_LIBRARY(kvitrayicon SHARED ${kvitrayicon_SRCS} ${kvitrayicon_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvitrayicon)

# Install target
SET_TARGET_PROPERTIES(kvitrayicon PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvitrayicon LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
