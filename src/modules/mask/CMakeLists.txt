# CMakeLists for src/modules/mask

SET(kvimask_SRCS
	libkvimask.cpp
)

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvimask MODULE ${kvimask_SRCS} ${kvimask_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvimask SHARED ${kvimask_SRCS} ${kvimask_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvimask ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvimask PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvimask LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
