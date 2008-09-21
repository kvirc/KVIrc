# CMakeLists for data/icons

FILE(GLOB files "*.svgz")

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/scalable/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/scalable/)
ENDIF()


