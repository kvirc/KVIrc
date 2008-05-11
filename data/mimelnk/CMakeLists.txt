# CMakeLists for data/mimelnk
IF(UNIX AND NOT APPLE)
	#INSTALL(FILES kvirc4.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/mimelnk/applications/)
	INSTALL(FILES x-kva.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/mimelnk/applications/)
	INSTALL(FILES x-kvt.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/mimelnk/applications/)
	INSTALL(FILES x-kvc.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/mimelnk/text/)
	INSTALL(FILES x-kvs.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/mimelnk/text/)
ENDIF()
