# CMakeLists for src/modules/mediaplayer

SET(kvimediaplayer_MOC_HDRS
	mp_amarokinterface.h
	mp_dcopinterface.h
	mp_interface.h
	mp_jukinterface.h
	mp_mp3.h
	mp_winampinterface.h
	mp_xmmsinterface.h
	mp_amipinterface.h
	mp_audaciousinterface.h
)

SET(kvimediaplayer_SRCS
	libkvimediaplayer.cpp
	mp_amarokinterface.cpp
	mp_dcopinterface.cpp
	mp_interface.cpp
	mp_jukinterface.cpp
	mp_mp3.cpp
	mp_winampinterface.cpp
	mp_xmmsinterface.cpp
	mp_amipinterface.cpp
	mp_audaciousinterface.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimediaplayer_MOC_SRCS ${kvimediaplayer_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvimediaplayer MODULE ${kvimediaplayer_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvimediaplayer SHARED ${kvimediaplayer_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvimediaplayer ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvimediaplayer kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvimediaplayer PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvimediaplayer LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvimediaplayer PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvimediaplayer LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSEIF(WIN32)
	SET_TARGET_PROPERTIES(kvimediaplayer PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvimediaplayer LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
