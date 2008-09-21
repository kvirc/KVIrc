# CMakeLists for data/icons

FILE(GLOB files "*.png")

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/32x32/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/32x32/)
ENDIF()


