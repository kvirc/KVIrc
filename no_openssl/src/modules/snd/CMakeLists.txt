# CMakeLists for src/modules/snd
IF(COMPILE_PHONON_SUPPORT)
	INCLUDE_DIRECTORIES(${PHONON_INCLUDE_DIR})
ENDIF()
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

SET(kvi_module_name kvisnd)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
