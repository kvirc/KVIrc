# CMakeLists for data/config

FILE(GLOB files "*.kvc")

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/config/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/config/)
	ENDIF()
ELSE()
	# Windows
	#INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/ ???  /config/)
ENDIF()
