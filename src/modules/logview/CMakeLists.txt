# CMakeLists for src/modules/logview

set(kvilogview_SRCS
	libkvilogview.cpp
	LogFile.cpp
	LogViewWidget.cpp
	LogViewWindow.cpp
)

set(kvi_module_name kvilogview)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
