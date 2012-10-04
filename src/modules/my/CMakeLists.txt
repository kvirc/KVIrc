# CMakeLists for src/modules/my

set(kvimy_SRCS
	libkvimy.cpp
	Idle_x11.cpp
	Idle_mac.cpp
	Idle.cpp
)

set(kvi_module_name kvimy)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
