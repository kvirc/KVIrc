# CMakeLists for src/modules/rijndael

SET(kvirijndael__MOC_HDRS
	libkvirijndael.h
	rijndael.h
	ablowfish.h
)

SET(kvirijndael_SRCS
	libkvirijndael.cpp
	rijndael.cpp
	ablowfish.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvirijndael_MOC_SRCS ${kvirijndael_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvirijndael MODULE ${kvirijndael_SRCS} ${kvirijndael_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvirijndael SHARED ${kvirijndael_SRCS} ${kvirijndael_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvirijndael ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvirijndael kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvirijndael PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvirijndael LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvirijndael PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvirijndael LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kvirijndael PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvirijndael LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
