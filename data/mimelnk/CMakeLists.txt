# CMakeLists for data/mimelnk
IF(UNIX AND NOT APPLE)
	INSTALL(FILES kvirc4.desktop DESTINATION ${INSTALL_PREFIX}/share/mimelnk/applications/)
	INSTALL(FILES x-kva.desktop DESTINATION ${INSTALL_PREFIX}/share/mimelnk/applications/)
	INSTALL(FILES x-kvt.desktop DESTINATION ${INSTALL_PREFIX}/share/mimelnk/applications/)
	INSTALL(FILES x-kvc.desktop DESTINATION ${INSTALL_PREFIX}/share/mimelnk/text/)
	INSTALL(FILES x-kvs.desktop DESTINATION ${INSTALL_PREFIX}/share/mimelnk/text/)
ENDIF()
