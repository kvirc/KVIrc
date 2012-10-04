# CMakeLists for src/modules/http

set(kvihttp_SRCS
	libkvihttp.cpp
	HttpFileTransfer.cpp
)

set(files
	kvi_httpicons.png
)

set(kvi_module_name kvihttp)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

if(UNIX)
	if(APPLE)
		install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/pics/)
	else()
		# Assume linux
		install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	endif()
elseif(WIN32)
	install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
endif()
