# CMakeLists for data/msgcolors

FILE(GLOB files "*.msgclr")
IF(APPLE)
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/msgcolors/)
ELSE()
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/msgcolors/)
ENDIF()
