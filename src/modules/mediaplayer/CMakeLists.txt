# CMakeLists for src/modules/mediaplayer

SET(kvimediaplayer_SRCS
	libkvimediaplayer.cpp
	mp_interface.cpp
	mp_mp3.cpp
	mp_winampinterface.cpp
	mp_xmmsinterface.cpp
	mp_amipinterface.cpp
	mp_mprisinterface.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimediaplayer_MOC_SRCS ${kvimediaplayer_MOC_HDRS})

SET(kvi_module_name kvimediaplayer)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

IF(WIN32)
	ADD_LIBRARY(gen_kvirc MODULE winamp.cpp)
	SET_TARGET_PROPERTIES(gen_kvirc PROPERTIES PREFIX "" LINK_FLAGS -static-libgcc COMPILE_FLAGS -static-libgcc)
	INSTALL(TARGETS gen_kvirc LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/modules)
ENDIF()
