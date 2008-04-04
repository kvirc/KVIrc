# CMakeLists for src/modules/torrent

SET(kvitorrent_MOC_HDRS
	tc_statusbarapplet.h
	tc_ktorrentdcopinterface.h
	tc_interface.h
)

SET(kvitorrent_SRCS
	libkvitorrent.cpp
	tc_statusbarapplet.cpp
	tc_ktorrentdcopinterface.cpp
	tc_interface.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitorrent_MOC_SRCS ${kvitorrent_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvitorrent MODULE ${kvitorrent_SRCS} ${kvitorrent_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvitorrent SHARED ${kvitorrent_SRCS} ${kvitorrent_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvitorrent ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvitorrent kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvitorrent PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvitorrent LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvitorrent PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvitorrent LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kvitorrent PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvitorrent LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
