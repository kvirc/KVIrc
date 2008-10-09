# CMakeLists for data/helppics

FILE(GLOB images "${CMAKE_CURRENT_SOURCE_DIR}/*.png")

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${images} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/help/en/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${images} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/help/en/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
ENDIF()
