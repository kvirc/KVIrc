# CMakeLists for src/modules/rot13

SUBDIRS(caps)

SET(kvirot13_MOC_HDRS
	libkvirot13.h
)

SET(kvirot13_SRCS
	libkvirot13.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvirot13_MOC_SRCS ${kvirot13_MOC_HDRS})

SET(kvi_module_name kvirot13)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
