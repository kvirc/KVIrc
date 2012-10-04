# CMakeLists for src/modules/toolbareditor

set(kvitoolbareditor_SRCS
	libkvitoolbareditor.cpp
	CustomizeToolBarsDialog.cpp
)

set(kvi_module_name kvitoolbareditor)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
