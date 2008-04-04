# CMakeLists for src/modules/lamerizer

SET(kvilamerizer_MOC_HDRS
	libkvilamerizer.h
)

SET(kvilamerizer_SRCS
	libkvilamerizer.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilamerizer_MOC_SRCS ${kvilamerizer_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvilamerizer MODULE ${kvilamerizer_SRCS} ${kvilamerizer_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvilamerizer SHARED ${kvilamerizer_SRCS} ${kvilamerizer_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvilamerizer ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvilamerizer kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvilamerizer PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvilamerizer LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvilamerizer PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvilamerizer LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kvilamerizer PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvilamerizer LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
