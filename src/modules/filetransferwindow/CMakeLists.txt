# CMakeLists for src/modules/filetransferwindow

set(kvifiletransferwindow_SRCS
	libkvifiletransferwindow.cpp
	FileTransferWindow.cpp
)

set(kvi_module_name kvifiletransferwindow)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
