# CMakeLists for src/modules/fish

SET(kvifish_MOC_HDRS
	libkvifish.h
)

SET(kvifish_SRCS
	libkvifish.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvifish_MOC_SRCS ${kvifish_MOC_HDRS})

SET(kvi_module_name kvifish)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
