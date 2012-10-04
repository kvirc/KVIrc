# CMakeLists for src/modules/snd

if(COMPILE_PHONON_SUPPORT)
	include_directories(${PHONON_INCLUDE_DIR})
endif()

if(MINGW)
	list(APPEND LIBS -lwinmm)
endif()

if(MSVC)
	list(APPEND LIBS winmm.lib)
endif()



set(kvisnd_SRCS
	libkvisnd.cpp
)

set(kvi_module_name kvisnd)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
