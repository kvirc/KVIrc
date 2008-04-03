# CMakeLists for data/defscript

FILE(GLOB files "*.kvs")

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/defscript/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/defscript/)
	ENDIF()
ELSE()
	# Windows
	#INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/defscript/)
ENDIF()
