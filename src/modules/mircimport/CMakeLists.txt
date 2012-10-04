# CMakeLists for src/modules/mircimport

subdirs(caps pics)



set(kvimircimport_SRCS
	libkvimircimport.cpp
)

set(kvi_module_name kvimircimport)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
