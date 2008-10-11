# CMakeLists for src/modules/iograph

SUBDIRS(caps)

SET(kviiograph_MOC_HDRS
	libkviiograph.h
)

SET(kviiograph_SRCS
	libkviiograph.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviiograph_MOC_SRCS ${kviiograph_MOC_HDRS})

SET(kvi_module_name kviiograph)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
