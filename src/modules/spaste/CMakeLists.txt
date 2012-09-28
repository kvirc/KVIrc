# CMakeLists for src/modules/spaste

SET(kvispaste_SRCS
	libkvispaste.cpp
	SlowPasteController.cpp
)

SET(kvi_module_name kvispaste)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
