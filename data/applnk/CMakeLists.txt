# CMakeLists for data/applnk

configure_file(
	${CMAKE_SOURCE_DIR}/cmake/net.kvirc.KVIrc.desktop.cmake
	${CMAKE_CURRENT_BINARY_DIR}/net.kvirc.KVIrc${CMAKE_KVIRC_VERSION_MAJOR}.desktop
)

if(UNIX)
	if(APPLE)
		install(
			FILES
				${CMAKE_CURRENT_BINARY_DIR}/net.kvirc.KVIrc${CMAKE_KVIRC_VERSION_MAJOR}.desktop
			DESTINATION
				${CMAKE_INSTALL_PREFIX}/Contents/Resources/applnk/
		)
	else()
		# Assume linux
		install(
			FILES
				${CMAKE_CURRENT_BINARY_DIR}/net.kvirc.KVIrc${CMAKE_KVIRC_VERSION_MAJOR}.desktop
			DESTINATION
				${CMAKE_INSTALL_PREFIX}/share/applications/
		)
	endif()
endif()
