# CMakeLists for src/modules/popupeditor

set(kvipopupeditor_SRCS
	libkvipopupeditor.cpp
	PopupEditorWindow.cpp
)

set(kvi_module_name kvipopupeditor)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
