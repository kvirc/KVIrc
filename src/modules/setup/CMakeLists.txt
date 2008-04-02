# CMakeLists for src/modules/setup

IF(WIN32)
	LIST(APPEND LIBS -lole32 -luuid)
ENDIF()

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
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvisetup MODULE ${kvisetup_SRCS} ${kvisetup_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvisetup SHARED ${kvisetup_SRCS} ${kvisetup_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvisetup ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvisetup PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvisetup LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
