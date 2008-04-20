# CMakeLists for src/modules/trayicon

SET(kvitrayicon_MOC_HDRS
	libkvitrayicon.h
)

SET(kvitrayicon_SRCS
	libkvitrayicon.cpp
)

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.png")

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitrayicon_MOC_SRCS ${kvitrayicon_MOC_HDRS})

SET(kvi_module_name kvitrayicon)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/pics/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${files} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/pics/)
ENDIF()