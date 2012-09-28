# CMakeLists for src/modules/codetester

SET(kvicodetester_SRCS
	libkvicodetester.cpp
	CodeTesterWindow.cpp
)

SET(kvi_module_name kvicodetester)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
