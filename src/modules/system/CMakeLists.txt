# CMakeLists for src/modules/system

SET(kvisystem_SRCS
	libkvisystem.cpp
	plugin.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisystem_MOC_SRCS ${kvisystem_MOC_HDRS})

SET(kvi_module_name kvisystem)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
