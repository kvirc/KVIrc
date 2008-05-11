# CMakeLists for src/modules/tip

SET(kvitip_MOC_HDRS
	libkvitip.h
)

SET(kvitip_SRCS
	libkvitip.cpp
)

FILE(GLOB tips "*.kvc")

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitip_MOC_SRCS ${kvitip_MOC_HDRS})

SET(kvi_module_name kvitip)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

# Install rule
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${tips} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/config/modules/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${tips} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/config/modules/)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${tips} DESTINATION ${CMAKE_INSTALL_PREFIX}/ ???  /config/modules/)
ENDIF()