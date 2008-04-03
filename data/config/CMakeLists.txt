# CMakeLists for data/config

FILE(GLOB files "*.kvc")
IF(APPLE)
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/config/)
ELSE()
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/config/)
ENDIF()
