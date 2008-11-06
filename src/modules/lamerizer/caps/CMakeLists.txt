# CMakeLists for ./caps

SET(CAPSFILE lamerizer)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/modules/caps/lamerizer/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/caps/lamerizer/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/modules/caps/lamerizer/)
ENDIF()
