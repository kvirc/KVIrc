# CMakeLists for src/modules/lamerizer

subdirs(caps)



set(kvilamerizer_SRCS
	libkvilamerizer.cpp
)

set(kvi_module_name kvilamerizer)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
