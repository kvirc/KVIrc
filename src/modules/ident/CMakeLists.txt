# CMakeLists for src/modules/ident

SET(kviident_SRCS
	libkviident.cpp
)

IF(WIN32)
	SET(LIBS ${LIBS} -lws2_32)
ENDIF(WIN32)

SET(kvi_module_name kviident)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
