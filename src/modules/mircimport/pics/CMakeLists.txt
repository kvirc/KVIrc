# CMakeLists for ./pics

set(PICSFILE kvi_mircimport.png)

if(UNIX)
	if(APPLE)
		install(FILES ${PICSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/pics/)
	else()
		# Assume linux
		install(FILES ${PICSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	endif()
elseif(WIN32)
	install(FILES ${CAPSFILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
endif()
