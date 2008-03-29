# CMakeLists for src/modules/setup

# From qmake
#win32 {
#	LIBS = -lole32 -luuid
#}

SET(kvieditor_MOC_HDRS
	setupwizard.h
)

SET(kvisetup_SRCS
	libkvisetup.cpp
	setupwizard.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisetup_MOC_SRCS ${kvisetup_MOC_HDRS})

# Create library
ADD_LIBRARY(kvisetup SHARED ${kvisetup_SRCS} ${kvisetup_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvisetup)

# Install target
SET_TARGET_PROPERTIES(kvisetup PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvisetup LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)