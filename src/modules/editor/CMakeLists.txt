# CMakeLists for src/modules/editor

SET(kvieditor_MOC_HDRS
	ScriptEditorImplementation.h
)

SET(kvieditor_SRCS
	libkvieditor.cpp
	ScriptEditorImplementation.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvieditor_MOC_SRCS ${kvieditor_MOC_HDRS})

SET(kvi_module_name kvieditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
