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
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvimask kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvimask PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvimask LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvimask PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvimask LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kvimask PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvimask LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
