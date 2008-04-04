# CMakeLists for src/modules/texticons

SET(kvitexticons_SRCS
	libkvitexticons.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitexticons_MOC_SRCS ${kvitexticons_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvitexticons MODULE ${kvitexticons_SRCS} ${kvitexticons_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvitexticons SHARED ${kvitexticons_SRCS} ${kvitexticons_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvitexticons ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvitexticons kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvitexticons PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvitexticons LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvitexticons PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvitexticons LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kvitexticons PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvitexticons LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
