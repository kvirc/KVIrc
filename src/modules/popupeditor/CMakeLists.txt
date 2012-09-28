# CMakeLists for src/modules/popupeditor

SET(kvipopupeditor_SRCS
	libkvipopupeditor.cpp
	PopupEditorWindow.cpp
)

SET(kvi_module_name kvipopupeditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
