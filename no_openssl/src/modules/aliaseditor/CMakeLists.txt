# CMakeLists for src/modules/aliaseditor

SET(kvialiaseditor_MOC_HDRS
	aliaseditor.h
)

SET(kvialiaseditor_SRCS
	libkvialiaseditor.cpp
	aliaseditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvialiaseditor_MOC_SRCS ${kvialiaseditor_MOC_HDRS})
SET(kvi_module_name kvialiaseditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
