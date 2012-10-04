# CMakeLists for src/modules/sharedfileswindow

set(kvisharedfileswindow_SRCS
	libkvisharedfileswindow.cpp
	SharedFilesWindow.cpp
)

set(kvi_module_name kvisharedfileswindow)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
