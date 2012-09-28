# CMakeLists for src/modules/help

SET(kvihelp_SRCS
	libkvihelp.cpp
	HelpWidget.cpp
	HelpWindow.cpp
	HelpIndex.cpp
)

SET(kvi_module_name kvihelp)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
