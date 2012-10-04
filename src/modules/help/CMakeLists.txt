# CMakeLists for src/modules/help

set(kvihelp_SRCS
	libkvihelp.cpp
	HelpWidget.cpp
	HelpWindow.cpp
	HelpIndex.cpp
)

set(kvi_module_name kvihelp)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
