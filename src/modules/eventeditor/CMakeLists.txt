# CMakeLists for src/modules/eventeditor

SET(kvieventeditor_MOC_HDRS
	EventEditorWindow.h
)

SET(kvieventeditor_SRCS
	EventEditorWindow.cpp
	libkvieventeditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvieventeditor_MOC_SRCS ${kvieventeditor_MOC_HDRS})

SET(kvi_module_name kvieventeditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
