# CMakeLists for data/icons/48x48/

SET(iconapps
	kvirc.png
)

SET(iconmime
	kvs.png
	kvc.png
	kvt.png
	kva.png
)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/48x48/apps/)
		INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/48x48/mimetypes/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps/)
		INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/mimetypes/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/48x48/apps/)
	INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/48x48/mimetypes/)
ENDIF()


