# CMakeLists for src/modules/socketspy

SET(kvisocketspy_SRCS
	libkvisocketspy.cpp
	SocketSpyWindow.cpp
)

SET(kvi_module_name kvisocketspy)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
