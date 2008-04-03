# CMakeLists for data/msgcolors

FILE(GLOB files "*.msgclr")

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/msgcolors/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/msgcolors/)
	ENDIF()
ELSE()
	# Windows
	#INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/msgcolors/)
ENDIF()
