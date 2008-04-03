# CMakeLists for data/pics/coresmall

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
IF(APPLE)
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/pics/coresmall)
ELSE()
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/coresmall)
ENDIF()
