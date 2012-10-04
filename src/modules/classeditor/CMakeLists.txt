# CMakeLists for src/modules/classeditor

set(kviclasseditor_SRCS
	libkviclasseditor.cpp
	ClassEditorWindow.cpp
)

set(kvi_module_name kviclasseditor)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
