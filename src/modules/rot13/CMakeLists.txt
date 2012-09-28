# CMakeLists for src/modules/rot13

SUBDIRS(caps)



SET(kvirot13_SRCS
	libkvirot13.cpp
)

SET(kvi_module_name kvirot13)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
