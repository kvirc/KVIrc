# CMakeLists for src/modules/perlcore

set(kviperlcore_SRCS
	libkviperlcore.cpp
)

set(kvi_module_name kviperlcore)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

if(COMPILE_PERL_SUPPORT)
	target_include_directories(${kvi_module_name} PRIVATE ${PERL_INCLUDE_DIRS})
	target_link_libraries(${kvi_module_name} ${PERL_LIBRARIES})
endif()
