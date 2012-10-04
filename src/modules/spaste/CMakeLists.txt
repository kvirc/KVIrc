# CMakeLists for src/modules/spaste

set(kvispaste_SRCS
	libkvispaste.cpp
	SlowPasteController.cpp
)

set(kvi_module_name kvispaste)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
