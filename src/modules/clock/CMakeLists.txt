# CMakeLists for src/modules/clock

SET(kviclock_MOC_HDRS
	libkviclock.h
)

SET(kviclock_SRCS
	libkviclock.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviclock_MOC_SRCS ${kviclock_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviclock MODULE ${kviclock_SRCS} ${kviclock_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviclock SHARED ${kviclock_SRCS} ${kviclock_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviclock ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kviclock kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kviclock PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kviclock LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kviclock PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kviclock LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSEIF(WIN32)
	SET_TARGET_PROPERTIES(kviclock PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
        #INSTALL(TARGETS kviclock LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
