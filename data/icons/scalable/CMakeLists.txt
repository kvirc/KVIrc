# CMakeLists for data/icons/scalable/

SET(iconapps
	kvirc.svgz
)

SET(iconmime
        text-x-kvs.svgz
        text-x-kvc.svgz
        application-x-kvt.svgz
        application-x-kva.svgz
)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/scalable/apps/)
		INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/icons/scalable/mimetypes/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps/)
		INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/mimetypes/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${iconapps} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/scalable/apps/)
	INSTALL(FILES ${iconmime} DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/scalable/mimetypes/)
ENDIF()


