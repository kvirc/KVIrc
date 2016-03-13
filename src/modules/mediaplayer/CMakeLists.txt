# CMakeLists for src/modules/mediaplayer

set(kvimediaplayer_SRCS
	libkvimediaplayer.cpp
	MpInterface.cpp
	MpMp3.cpp
	MpWinampInterface.cpp
	MpXmmsInterface.cpp
	MpAmipInterface.cpp
	MpMprisInterface.cpp
)

set(kvi_module_name kvimediaplayer)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

if(WIN32)
	add_library(gen_kvirc MODULE winamp.cpp)
	if(MINGW)
		set_target_properties(gen_kvirc PROPERTIES PREFIX "" LINK_FLAGS -static-libgcc COMPILE_FLAGS -static-libgcc)
	endif()
	install(TARGETS gen_kvirc LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/modules)
endif()
