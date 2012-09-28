# CMakeLists for src/modules/window

SET(kviwindow_SRCS
	libkviwindow.cpp
	UserWindow.cpp
)

SET(kvi_module_name kviwindow)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
