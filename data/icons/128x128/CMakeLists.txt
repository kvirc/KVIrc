# CMakeLists for data/icons

FILE(GLOB files "*.png")

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/128x128/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/128x128/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/128x128/)
ENDIF()


