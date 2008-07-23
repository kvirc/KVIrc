# CMakeLists for src/modules/mediaplayer

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
	mp_mprisinterface.cpp
	mp_xmms2interface.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimediaplayer_MOC_SRCS ${kvimediaplayer_MOC_HDRS})

SET(kvi_module_name kvimediaplayer)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
