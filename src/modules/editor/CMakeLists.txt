# CMakeLists for src/modules/editor

set(kvieditor_SRCS
	libkvieditor.cpp
	ScriptEditorImplementation.cpp
)

set(kvi_module_name kvieditor)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
