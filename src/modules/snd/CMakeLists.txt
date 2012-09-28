# CMakeLists for src/modules/snd

IF(COMPILE_PHONON_SUPPORT)
	INCLUDE_DIRECTORIES(${PHONON_INCLUDE_DIR})
ENDIF()

IF(MINGW)
	LIST(APPEND LIBS -lwinmm)
ENDIF()

IF(MSVC)
	LIST(APPEND LIBS winmm.lib)
ENDIF()



SET(kvisnd_SRCS
	libkvisnd.cpp
)

SET(kvi_module_name kvisnd)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
