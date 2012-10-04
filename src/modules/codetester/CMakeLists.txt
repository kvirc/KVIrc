# CMakeLists for src/modules/codetester

set(kvicodetester_SRCS
	libkvicodetester.cpp
	CodeTesterWindow.cpp
)

set(kvi_module_name kvicodetester)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
