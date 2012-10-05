# CMakeLists for src/modules/ident

set(kviident_SRCS
	libkviident.cpp
)

if(MINGW)
	list(APPEND LIBS -lws2_32)
endif(MINGW)

set(kvi_module_name kviident)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
