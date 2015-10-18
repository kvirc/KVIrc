# CMakeLists for data/icons/scalable/

set(iconapps
	kvirc.svgz
)

set(iconmime
        text-x-kvs.svgz
        text-x-kvc.svgz
        application-x-kvt.svgz
        application-x-kva.svgz
)

if(UNIX)
	if(APPLE)
		install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/scalable/apps/)
		install(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/scalable/mimetypes/)
	else()
		# Assume linux
		install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps/)
		install(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/mimetypes/)
	endif()
elseif(WIN32)
	install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/scalable/apps/)
	install(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/scalable/mimetypes/)
endif()
