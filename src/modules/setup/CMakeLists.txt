# CMakeLists for src/modules/setup

IF(WIN32)
	LIST(APPEND LIBS -lole32 -luuid)
ENDIF()

SET(kvisetup_MOC_HDRS
	setupwizard.h
)

SET(kvisetup_SRCS
	libkvisetup.cpp
	setupwizard.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisetup_MOC_SRCS ${kvisetup_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvisetup MODULE ${kvisetup_SRCS} ${kvisetup_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvisetup SHARED ${kvisetup_SRCS} ${kvisetup_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvisetup ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvisetup kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvisetup PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvisetup LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvisetup PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvisetup LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kvisetup PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvisetup LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
