# CMakeLists for data/icons/48x48/

set(iconapps
	kvirc.png
)

set(iconmime
        text-x-kvs.png
        text-x-kvc.png
        application-x-kvt.png
        application-x-kva.png
)

if(UNIX)
	if(APPLE)
		install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/48x48/apps/)
		install(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/48x48/mimetypes/)
	else()
		# Assume linux
		install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps/)
		install(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/mimetypes/)
		#this is needed for .desktop files to find the icon
		install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/)
	endif()
elseif(WIN32)
	install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/48x48/apps/)
	install(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/48x48/mimetypes/)
endif()
