# CMakeLists for src/modules/sharedfile

SET(kvisharedfile_SRCS
	libkvisharedfile.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisharedfile_MOC_SRCS ${kvisharedfile_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvisharedfile MODULE ${kvisharedfile_SRCS} ${kvisharedfile_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvisharedfile SHARED ${kvisharedfile_SRCS} ${kvisharedfile_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvisharedfile ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvisharedfile PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvisharedfile LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
