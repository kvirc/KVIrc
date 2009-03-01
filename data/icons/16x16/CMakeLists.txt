# CMakeLists for data/icons/16x16/

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
		INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/16x16/apps/)
		INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/16x16/mimetypes/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/16x16/apps/)
		INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/16x16/mimetypes/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/16x16/apps/)
	INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/16x16/mimetypes/)
ENDIF()


