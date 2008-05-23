# CMakeLists for data/applnk

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.desktop")

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/applnk/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/applnk/)
ENDIF()
