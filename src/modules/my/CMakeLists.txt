# CMakeLists for src/modules/my

SET(kvimy_MOC_HDRS
	idle.h
)

SET(kvimy_SRCS
	libkvimy.cpp
	idle_x11.cpp
	idle_mac.cpp
	idle.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimy_MOC_SRCS ${kvimy_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvimy MODULE ${kvimy_SRCS} ${kvimy_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvimy SHARED ${kvimy_SRCS} ${kvimy_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvimy ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvimy kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvimy PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvimy LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvimy PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvimy LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSEIF(WIN32)
	SET_TARGET_PROPERTIES(kvimy PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvimy LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
