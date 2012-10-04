# CMakeLists for src/modules/system

set(kvisystem_SRCS
	libkvisystem.cpp
	Plugin.cpp
)

set(kvi_module_name kvisystem)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
