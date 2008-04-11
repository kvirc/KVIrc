# CMakeLists for src/modules/config

SET(kviconfig_SRCS
	libkviconfig.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviconfig_MOC_SRCS ${kviconfig_MOC_HDRS})

SET(kvi_module_name kviconfig)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
