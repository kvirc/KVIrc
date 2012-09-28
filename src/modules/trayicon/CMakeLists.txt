# CMakeLists for src/modules/trayicon

SET(kvitrayicon_SRCS
	libkvitrayicon.cpp
)

SET(unixFiles
	kvi_dock.png
	kvi_dock_part-0.png
	kvi_dock_part-1.png
	kvi_dock_part-2.png
)

SET(win32Files
	kvi_dock_win32.png
	kvi_dock_win32-0.png
	kvi_dock_win32-1.png
	kvi_dock_win32-2.png
)

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.png")

SET(kvi_module_name kvitrayicon)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${unixFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/pics/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${unixFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${win32Files} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
ENDIF()