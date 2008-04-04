# CMakeLists for src/modules/snd

IF(WIN32)
	LIST(APPEND LIBS -lwinmm)
ENDIF()

SET(kvisnd_MOC_HDRS
	libkvisnd.h
)

SET(kvisnd_SRCS
	libkvisnd.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisnd_MOC_SRCS ${kvisnd_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvisnd MODULE ${kvisnd_SRCS} ${kvisnd_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvisnd SHARED ${kvisnd_SRCS} ${kvisnd_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvisnd ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvisnd kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvisnd PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvisnd LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvisnd PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvisnd LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSEIF(WIN32)
	SET_TARGET_PROPERTIES(kvisnd PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvisnd LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
