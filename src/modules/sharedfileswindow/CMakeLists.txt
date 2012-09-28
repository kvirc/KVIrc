# CMakeLists for src/modules/sharedfileswindow

SET(kvisharedfileswindow_SRCS
	libkvisharedfileswindow.cpp
	SharedFilesWindow.cpp
)

SET(kvi_module_name kvisharedfileswindow)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
