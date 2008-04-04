# CMakeLists for src/modules/str

IF(UNIX)
	IF(APPLE)
		LIST(APPEND LIBS -lssl -lcrypto)
	ENDIF()
ELSE()
	# Assume windows
	LIST(APPEND LIBS -leay32 -lssleay32)
ENDIF()

SET(kvistr_SRCS
	libkvistr.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvistr_MOC_SRCS ${kvistr_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvistr MODULE ${kvistr_SRCS} ${kvistr_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvistr SHARED ${kvistr_SRCS} ${kvistr_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvistr ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvistr kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvistr PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvistr LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvistr PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvistr LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSEIF(WIN32)
	SET_TARGET_PROPERTIES(kvistr PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvistr LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
