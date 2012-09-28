# CMakeLists for src/modules/system

SET(kvisystem_SRCS
	libkvisystem.cpp
	Plugin.cpp
)

SET(kvi_module_name kvisystem)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
