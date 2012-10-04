# CMakeLists for data/resources_mac

if(APPLE)
	if(WANT_COEXISTENCE)
		set(KVIRC_BINARYNAME ${PACKAGE}${VERSION_MAJOR})
	else()
		set(KVIRC_BINARYNAME ${PACKAGE})
	endif()

	configure_file(${kvirc_SOURCE_DIR}/cmake/Info.plist.cmake ${CMAKE_CURRENT_BINARY_DIR}/Info.plist @ONLY)
	install(FILES kvirc.icns DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Info.plist DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/)
endif()
