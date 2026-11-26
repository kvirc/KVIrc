# CMakeLists for src/modules/mircimport

add_subdirectory(caps)
add_subdirectory(pics)



set(kvimircimport_SRCS
	libkvimircimport.cpp
)

set(kvi_module_name kvimircimport)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
