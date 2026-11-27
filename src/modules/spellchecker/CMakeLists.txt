# CMakeLists for src/modules/avatar

set(kvispellchecker_SRCS
	libkvispellchecker.cpp
)

set(kvi_module_name kvispellchecker)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

if(COMPILE_ENCHANT_SUPPORT)
	target_link_libraries(${kvi_module_name} PkgConfig::Enchant)
endif()
