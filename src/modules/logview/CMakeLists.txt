# CMakeLists for src/modules/logview

SET(kvilogview_SRCS
	libkvilogview.cpp
	LogFile.cpp
	LogViewWidget.cpp
	LogViewWindow.cpp
)

SET(kvi_module_name kvilogview)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

