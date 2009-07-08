# CMakeLists for src/modules/actioneditor

SET(kviactioneditor_MOC_HDRS
	actioneditor.h
)

SET(kviactioneditor_SRCS
	actioneditor.cpp
	libkviactioneditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviactioneditor_MOC_SRCS ${kviactioneditor_MOC_HDRS})

SET(kvi_module_name kviactioneditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
