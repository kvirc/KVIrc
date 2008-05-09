# CMakeLists for src/modules/mircimport

SUBDIRS(caps pics)

SET(kvimircimport_MOC_HDRS
	libkvimircimport.h
)

SET(kvimircimport_SRCS
	libkvimircimport.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimircimport_MOC_SRCS ${kvimircimport_MOC_HDRS})

SET(kvi_module_name kvimircimport)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
