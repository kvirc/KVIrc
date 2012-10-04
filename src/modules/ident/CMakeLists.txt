# CMakeLists for src/modules/ident

set(kviident_SRCS
	libkviident.cpp
)

if(WIN32)
	set(LIBS ${LIBS} -lws2_32)
endif(WIN32)

set(kvi_module_name kviident)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
