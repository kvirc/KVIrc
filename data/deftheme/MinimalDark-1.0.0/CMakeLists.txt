# CMakeLists for data/deftheme/MinimalDark-1.0.0

file(GLOB images "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
file(GLOB themefiles "${CMAKE_CURRENT_SOURCE_DIR}/*.kvc")
list(APPEND files ${images} ${themefiles})

if(UNIX)
	if(APPLE)
		install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/themes/MinimalDark-1.0.0/)
	else()
		# Assume linux
		install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/themes/MinimalDark-1.0.0/)
	endif()
elseif(WIN32)
	install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/themes/MinimalDark-1.0.0/)
endif()
