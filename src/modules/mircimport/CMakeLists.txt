# CMakeLists for src/modules/mircimport

SUBDIRS(caps pics)



SET(kvimircimport_SRCS
	libkvimircimport.cpp
)

SET(kvi_module_name kvimircimport)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
