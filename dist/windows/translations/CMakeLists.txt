# CMakeLists for dist/windows/translations

file(GLOB translations "${CMAKE_CURRENT_SOURCE_DIR}/*.nsi")

if(WIN32)
	install(FILES ${translations} DESTINATION ${PROJECT_BINARY_DIR}/translations/)
endif()
