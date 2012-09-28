# CMakeLists for src/modules/term

SET(kviterm_SRCS
	libkviterm.cpp
	TermWidget.cpp
	TermWindow.cpp
)

SET(kvi_module_name kviterm)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
