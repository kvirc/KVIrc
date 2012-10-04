# CMakeLists for data/applnk

configure_file(${CMAKE_SOURCE_DIR}/cmake/kvirc4.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/${KVIRC_BINARYNAME}.desktop)

if(UNIX)
	if(APPLE)
		install(FILES
			${CMAKE_CURRENT_BINARY_DIR}/${KVIRC_BINARYNAME}.desktop
			DESTINATION
			${CMAKE_INSTALL_PREFIX}/Contents/Resources/applnk/
		)
	else()
		# Assume linux
		install(FILES
			${CMAKE_CURRENT_BINARY_DIR}/${KVIRC_BINARYNAME}.desktop
			DESTINATION
			${CMAKE_INSTALL_PREFIX}/share/applications/
		)
	endif()
endif()
