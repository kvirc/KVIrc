# CMakeLists for ./caps

SET(CAPSFILE mircimport)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/modules/caps/serverimport/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/caps/serverimport/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/modules/caps/serverimport/)
ENDIF()
