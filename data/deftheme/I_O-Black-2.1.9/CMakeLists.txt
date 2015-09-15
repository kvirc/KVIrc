# CMakeLists for data/deftheme/I_O-Black-2.1.9

file(GLOB images "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
file(GLOB themefiles "${CMAKE_CURRENT_SOURCE_DIR}/*.kvc")
list(APPEND files ${images} ${themefiles})

if(UNIX)
	if(APPLE)
		install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/themes/I_O-Black-2.1.9/)
	else()
		# Assume Linux
		install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/themes/I_O-Black-2.1.9/)
	endif()
elseif(WIN32)
	install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/themes/I_O-Black-2.1.9/)
endif()
