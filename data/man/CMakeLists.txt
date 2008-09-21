# CMakeLists for data/man

FILE(GLOB files "*.1")

IF(UNIX)
	IF(APPLE)
		# does macosx use manpages?
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1/)
	ENDIF()
ENDIF()


