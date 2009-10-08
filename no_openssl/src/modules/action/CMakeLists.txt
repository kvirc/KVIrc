# CMakeLists for src/modules/action

SET(kviaction_SRCS
	libkviaction.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviaction_MOC_SRCS ${kviaction_MOC_HDRS})
SET(kvi_module_name kviaction)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
