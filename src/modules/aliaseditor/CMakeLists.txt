# CMakeLists for src/modules/aliaseditor

set(kvialiaseditor_SRCS
	libkvialiaseditor.cpp
	AliasEditorWindow.cpp
)

set(kvi_module_name kvialiaseditor)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
