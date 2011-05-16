# CMakeLists for src/modules/mediaplayer

SET(kvimediaplayer_SRCS
	libkvimediaplayer.cpp
	MpInterface.cpp
	MpMp3.cpp
	MpWinampInterface.cpp
	MpXmmsInterface.cpp
	MpAmipInterface.cpp
	MpMprisInterface.cpp
	MpSpotifyInterface.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimediaplayer_MOC_SRCS ${kvimediaplayer_MOC_HDRS})

SET(kvi_module_name kvimediaplayer)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

IF(WIN32)
	#dirty workaround for the QString::fromWCharArray() use in the spotify interface.
	#TODO: put it in the CMakeLists.txt in / and see if it breaks something.
	IF (MSVC)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
	ENDIF()

	ADD_LIBRARY(gen_kvirc MODULE winamp.cpp)
	SET_TARGET_PROPERTIES(gen_kvirc PROPERTIES PREFIX "" LINK_FLAGS -static-libgcc COMPILE_FLAGS -static-libgcc)
	INSTALL(TARGETS gen_kvirc LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/modules)
ENDIF()
