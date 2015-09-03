# CMakeLists for src/modules/trayicon

set(kvitrayicon_SRCS
	libkvitrayicon.cpp
)

set(imageFiles
	kvi_dock_normal_16-0.png
	kvi_dock_normal_16-1.png
	kvi_dock_normal_16-2.png
	kvi_dock_normal_22-0.png
	kvi_dock_normal_22-1.png
	kvi_dock_normal_22-2.png
	kvi_dock_normal_48-0.png
	kvi_dock_normal_48-1.png
	kvi_dock_normal_48-2.png
	kvi_dock_mono_22-0.png
	kvi_dock_mono_22-1.png
	kvi_dock_mono_22-2.png
	kvi_dock_mono_48-0.png
	kvi_dock_mono_48-1.png
	kvi_dock_mono_48-2.png
)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.png")

set(kvi_module_name kvitrayicon)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

if(UNIX)
	if(APPLE)
		install(FILES ${imageFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/pics/)
	else()
		install(FILES ${imageFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	endif()
elseif(WIN32)
	install(FILES ${imageFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
endif()
