# CMakeLists for src/modules/my

SET(kvimy_SRCS
	libkvimy.cpp
	Idle_x11.cpp
	Idle_mac.cpp
	Idle.cpp
)

SET(kvi_module_name kvimy)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
