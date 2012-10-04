# CMakeLists for src/modules/trayicon

set(kvitrayicon_SRCS
	libkvitrayicon.cpp
)

set(unixFiles
	kvi_dock.png
	kvi_dock_part-0.png
	kvi_dock_part-1.png
	kvi_dock_part-2.png
)

set(win32Files
	kvi_dock_win32.png
	kvi_dock_win32-0.png
	kvi_dock_win32-1.png
	kvi_dock_win32-2.png
)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.png")

set(kvi_module_name kvitrayicon)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

if(UNIX)
	if(APPLE)
		install(FILES ${unixFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/pics/)
	else()
		# Assume linux
		install(FILES ${unixFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	endif()
elseif(WIN32)
	install(FILES ${win32Files} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
endif()
