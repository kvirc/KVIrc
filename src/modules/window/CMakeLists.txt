# CMakeLists for src/modules/window

set(kviwindow_SRCS
	libkviwindow.cpp
	UserWindow.cpp
)

set(kvi_module_name kviwindow)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
