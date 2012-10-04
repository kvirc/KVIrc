# CMakeLists for data/helppics

file(GLOB images "${CMAKE_CURRENT_SOURCE_DIR}/*.png")

if(UNIX)
	if(APPLE)
		install(FILES ${images} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/help/en/)
	else()
		# Assume linux
		install(FILES ${images} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/help/en/)
	endif()
elseif(WIN32)
	install(FILES ${images} DESTINATION ${CMAKE_INSTALL_PREFIX}/help/en/)
endif()
