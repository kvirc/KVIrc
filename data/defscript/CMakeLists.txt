# CMakeLists for data/defscript

FILE(GLOB files "*.kvs")
IF(APPLE)
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/defscript/)
ELSE()
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/defscript/)
ENDIF()
