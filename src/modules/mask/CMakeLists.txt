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
SET_TARGET_PROPERTIES(kvimask PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kvimask LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
