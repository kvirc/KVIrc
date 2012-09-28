# CMakeLists for src/modules/classeditor

SET(kviclasseditor_SRCS
	libkviclasseditor.cpp
	ClassEditorWindow.cpp
)

SET(kvi_module_name kviclasseditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
