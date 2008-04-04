# CMakeLists for src/modules/trayicon

SET(kvitrayicon_MOC_HDRS
	libkvitrayicon.h
	trayicon.h
)

SET(kvitrayicon_SRCS
	libkvitrayicon.cpp
	trayicon.cpp
	trayicon_x11.cpp
	trayicon_mac.cpp
	trayicon_win.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitrayicon_MOC_SRCS ${kvitrayicon_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvitrayicon MODULE ${kvitrayicon_SRCS} ${kvitrayicon_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvitrayicon SHARED ${kvitrayicon_SRCS} ${kvitrayicon_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvitrayicon ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvitrayicon kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvitrayicon PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvitrayicon LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvitrayicon PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvitrayicon LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kvitrayicon PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvitrayicon LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
