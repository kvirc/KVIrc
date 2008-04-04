# CMakeLists for data/resources_mac
IF(APPLE)
                INSTALL(FILES kvirc.icns DESTINATION ${INSTALL_PREFIX}/Contents/Resources/)
                INSTALL(FILES Info.plist DESTINATION ${INSTALL_PREFIX}/Contents/)
ENDIF()
