# CMakeLists for win32build
SUBDIRS(translations)

SET(files
	README.txt
)

IF(WIN32)
	#attention: nsis scripts contains ${variables} that would be replaced by cmake
        CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/KVIrc.nsi.cmake ${PROJECT_BINARY_DIR}/KVIrc.nsi @ONLY)
	INSTALL(FILES ${files} DESTINATION ${PROJECT_BINARY_DIR}/)
ENDIF()