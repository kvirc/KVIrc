# CMakeLists for data/icons/48x48/

SET(iconapps
	kvirc.png
)

SET(iconmime
        text-x-kvs.png
        text-x-kvc.png
        application-x-kvt.png
        application-x-kva.png
)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/48x48/apps/)
		INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/48x48/mimetypes/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps/)
		INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/mimetypes/)
		#this is needed for .desktop files to find the icon
		INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/48x48/apps/)
	INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/48x48/mimetypes/)
ENDIF()


