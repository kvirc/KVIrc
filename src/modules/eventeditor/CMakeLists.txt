# CMakeLists for src/modules/eventeditor

set(kvieventeditor_SRCS
	EventEditorWindow.cpp
	libkvieventeditor.cpp
)

set(kvi_module_name kvieventeditor)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
