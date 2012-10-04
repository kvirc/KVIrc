# CMakeLists for src/modules/about

set(kviabout_SRCS
	AboutDialog.cpp
	libkviabout.cpp
)

set(kvi_module_name kviabout)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
