# CMakeLists for dist/windows

subdirs(translations)

if(WIN32)
	#attention: nsis scripts contains ${variables} that would be replaced by cmake
        configure_file(${PROJECT_SOURCE_DIR}/cmake/KVIrc.nsi.cmake ${PROJECT_BINARY_DIR}/KVIrc.nsi @ONLY)
endif()
