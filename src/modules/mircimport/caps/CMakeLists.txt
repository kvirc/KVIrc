# CMakeLists for ./caps

set(CAPSFILE mircimport)

if(UNIX)
	if(APPLE)
		install(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/modules/caps/serverimport/)
	else()
		# Assume linux
		install(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/caps/serverimport/)
	endif()
elseif(WIN32)
	install(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/modules/caps/serverimport/)
endif()
