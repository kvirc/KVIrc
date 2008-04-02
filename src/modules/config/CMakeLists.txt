# CMakeLists for src/modules/config

SET(kviconfig_SRCS
	libkviconfig.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviconfig_MOC_SRCS ${kviconfig_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviconfig MODULE ${kviconfig_SRCS} ${kviconfig_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviconfig SHARED ${kviconfig_SRCS} ${kviconfig_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviconfig ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviconfig PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kviconfig LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
