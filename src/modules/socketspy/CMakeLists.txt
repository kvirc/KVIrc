# CMakeLists for src/modules/socketspy

set(kvisocketspy_SRCS
	libkvisocketspy.cpp
	SocketSpyWindow.cpp
)

set(kvi_module_name kvisocketspy)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
