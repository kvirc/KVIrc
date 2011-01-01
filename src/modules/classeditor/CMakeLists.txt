# CMakeLists for src/modules/classeditor

SET(kviclasseditor_MOC_HDRS
	ClassEditorWindow.h
)

SET(kviclasseditor_SRCS
	libkviclasseditor.cpp
	ClassEditorWindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviclasseditor_MOC_SRCS ${kviclasseditor_MOC_HDRS})
SET(kvi_module_name kviclasseditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
