# CMakeLists for src/modules/lamerizer

SUBDIRS(caps)

SET(kvilamerizer_MOC_HDRS
	libkvilamerizer.h
)

SET(kvilamerizer_SRCS
	libkvilamerizer.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilamerizer_MOC_SRCS ${kvilamerizer_MOC_HDRS})

SET(kvi_module_name kvilamerizer)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
