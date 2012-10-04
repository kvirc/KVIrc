# CMakeLists for src/modules/language

set(kvilanguage_SRCS
	libkvilanguage.cpp
	detector.cpp
)

set(kvi_module_name kvilanguage)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
