# CMakeLists for src/modules/links

set(kvilinks_SRCS
	libkvilinks.cpp
	LinksWindow.cpp
)

set(kvi_module_name kvilinks)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
