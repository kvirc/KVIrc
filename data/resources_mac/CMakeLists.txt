# CMakeLists for data/resources_mac
IF(APPLE)
                INSTALL(FILES kvirc.icns DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/)
                INSTALL(FILES Info.plist DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/)
ENDIF()
