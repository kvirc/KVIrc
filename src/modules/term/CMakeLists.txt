# CMakeLists for src/modules/term

set(kviterm_SRCS
	libkviterm.cpp
	TermWidget.cpp
	TermWindow.cpp
)

set(kvi_module_name kviterm)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
