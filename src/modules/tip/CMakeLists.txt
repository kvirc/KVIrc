# CMakeLists for src/modules/tip

set(kvitip_SRCS
	libkvitip.cpp
)

file(GLOB tips "*.kvc")
file(GLOB pics "*.png")

set(kvi_module_name kvitip)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

# Install rule
if(UNIX)
	if(APPLE)
		install(FILES ${tips} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/config/modules/)
		install(FILES ${pics} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/pics/)
	else()
		# Assume linux
		install(FILES ${tips} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/config/modules/)
		install(FILES ${pics} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	endif()
elseif(WIN32)
	install(FILES ${tips} DESTINATION ${CMAKE_INSTALL_PREFIX}/config/modules/)
	install(FILES ${pics} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
endif()
