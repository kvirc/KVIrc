# CMakeLists for src/modules/editor

SET(kvieditor_SRCS
	libkvieditor.cpp
	ScriptEditorImplementation.cpp
)

SET(kvi_module_name kvieditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
