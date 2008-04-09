# CMakeLists for doc/scriptexamples/playing/

SET(playing
	playing.kvs
	playing_icon.png
)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${playing} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/doc/scriptexamples/playing)
	ELSE()
		# Assume linux
		INSTALL(FILES ${playing} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/scriptexamples/playing)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${playing} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/doc/scriptexamples/playing)
ENDIF()
