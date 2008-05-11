# CMakeLists for doc/scriptexamples/kvisounds/
SUBDIRS(waves)

SET(kvisounds kvisounds.kvs)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${kvisounds} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/doc/scriptexamples/kvisounds)
	ELSE()
		# Assume linux
		INSTALL(FILES ${kvisounds} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/scriptexamples/kvisounds)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${kvisounds} DESTINATION ${CMAKE_INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/doc/scriptexamples/kvisounds)
ENDIF()
