# CMakeLists for src/modules/filetransferwindow/caps

SET(CAPSFILE filetransferwindow)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/modules/caps/tool/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/caps/tool/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/modules/caps/tool/)
ENDIF()
