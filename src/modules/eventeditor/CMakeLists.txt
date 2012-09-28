# CMakeLists for src/modules/eventeditor

SET(kvieventeditor_SRCS
	EventEditorWindow.cpp
	libkvieventeditor.cpp
)

SET(kvi_module_name kvieventeditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
