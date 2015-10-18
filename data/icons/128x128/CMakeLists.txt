# CMakeLists for data/icons/128x128/

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
		install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/128x128/apps/)
		install(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/128x128/mimetypes/)
	else()
		# Assume linux
		install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/128x128/apps/)
		install(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/128x128/mimetypes/)
	endif()
elseif(WIN32)
	install(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/128x128/apps/)
	install(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/128x128/mimetypes/)
endif()
