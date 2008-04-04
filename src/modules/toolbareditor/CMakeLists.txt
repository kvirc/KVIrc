# CMakeLists for src/modules/toolbareditor

SET(kvitoolbareditor_MOC_HDRS
	toolbareditor.h
)

SET(kvitoolbareditor_SRCS
	libkvitoolbareditor.cpp
	toolbareditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitoolbareditor_MOC_SRCS ${kvitoolbareditor_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvitoolbareditor MODULE ${kvitoolbareditor_SRCS} ${kvitoolbareditor_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvitoolbareditor SHARED ${kvitoolbareditor_SRCS} ${kvitoolbareditor_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvitoolbareditor ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvitoolbareditor kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvitoolbareditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvitoolbareditor LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvitoolbareditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvitoolbareditor LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kvitoolbareditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvitoolbareditor LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
