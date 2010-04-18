# CMakeLists for win32build
SUBDIRS(translations)

IF(WIN32)
	#attention: nsis scripts contains ${variables} that would be replaced by cmake
        CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/KVIrc.nsi.cmake ${PROJECT_BINARY_DIR}/KVIrc.nsi @ONLY)
ENDIF()