# CMakeLists for src/modules/perl

SET(kviperl_SRCS
	libkviperl.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviperl_MOC_SRCS ${kviperl_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviperl MODULE ${kviperl_SRCS} ${kviperl_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviperl SHARED ${kviperl_SRCS} ${kviperl_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviperl ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviperl PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kviperl LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
