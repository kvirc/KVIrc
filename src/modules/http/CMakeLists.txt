# CMakeLists for src/modules/http

SET(kvihttp_MOC_HDRS
	HttpFileTransfer.h
)

SET(kvihttp_SRCS
	libkvihttp.cpp
	HttpFileTransfer.cpp
)

SET(files
	kvi_httpicons.png
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvihttp_MOC_SRCS ${kvihttp_MOC_HDRS})

SET(kvi_module_name kvihttp)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/pics/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
ENDIF()