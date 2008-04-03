# CMakeLists for data/pics

SUBDIRS(coresmall)

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
IF(APPLE)
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/pics/)
ELSE()
	INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
ENDIF()
