# CMakeLists for src/modules/rot13

add_subdirectory(caps)



set(kvirot13_SRCS
	libkvirot13.cpp
)

set(kvi_module_name kvirot13)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
