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
ADD_LIBRARY(kvimediaplayer SHARED ${kvimediaplayer_SRCS} ${kvimediaplayer_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviaddon)

# Install target
SET_TARGET_PROPERTIES(kvimediaplayer PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvimediaplayer LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)