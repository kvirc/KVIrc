# CMakeLists for src/modules/raweditor

set(kviraweditor_SRCS
	libkviraweditor.cpp
	RawEditorWindow.cpp
)

set(kvi_module_name kviraweditor)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
