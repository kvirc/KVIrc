# CMakeLists for src/modules/ident

SET(kviident_MOC_HDRS
	libkviident.h
)

SET(kviident_SRCS
	libkviident.cpp
)

IF(WIN32)
	SET(LIBS ${LIBS} -lws2_32)
ENDIF(WIN32)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviident_MOC_SRCS ${kviident_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviident MODULE ${kviident_SRCS} ${kviident_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviident SHARED ${kviident_SRCS} ${kviident_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviident ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kviident kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kviident PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kviident LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kviident PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kviident LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kviident PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kviident LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
