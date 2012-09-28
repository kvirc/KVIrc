# CMakeLists for src/modules/lamerizer

SUBDIRS(caps)



SET(kvilamerizer_SRCS
	libkvilamerizer.cpp
)

SET(kvi_module_name kvilamerizer)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
