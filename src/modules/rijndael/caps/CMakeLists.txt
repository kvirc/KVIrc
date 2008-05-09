# CMakeLists for ./caps

SET(CAPSFILE rijndael)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${CAPSFILE} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/caps/crypt/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${CAPSFILE} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/caps/crypt/)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${CAPSFILE} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/caps/crypt/)
ENDIF()
