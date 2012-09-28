# CMakeLists for src/modules/language

SET(kvilanguage_SRCS
	libkvilanguage.cpp
	detector.cpp
)

SET(kvi_module_name kvilanguage)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
