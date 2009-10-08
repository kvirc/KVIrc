# CMakeLists for win32build/translations

FILE(GLOB translations "${CMAKE_CURRENT_SOURCE_DIR}/*.nsi")

IF(WIN32)
	INSTALL(FILES ${translations} DESTINATION ${PROJECT_BINARY_DIR}/translations/)
ENDIF()