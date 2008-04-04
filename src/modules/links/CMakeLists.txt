# CMakeLists for src/modules/links

SET(kvilinks_MOC_HDRS
	linkswindow.h
)

SET(kvilinks_SRCS
	libkvilinks.cpp
	linkswindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilinks_MOC_SRCS ${kvilinks_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvilinks MODULE ${kvilinks_SRCS} ${kvilinks_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvilinks SHARED ${kvilinks_SRCS} ${kvilinks_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvilinks ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvilinks kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvilinks PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvilinks LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvilinks PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvilinks LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSEIF(WIN32)
	SET_TARGET_PROPERTIES(kvilinks PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvilinks LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
