# CMakeLists for ./caps

set(CAPSFILE rot13)

if(UNIX)
	if(APPLE)
		install(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/modules/caps/crypt/)
	else()
		# Assume linux
		install(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/caps/crypt/)
	endif()
elseif(WIN32)
	install(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/modules/caps/crypt/)
endif()
