# CMakeLists for src/modules/toolbareditor

SET(kvitoolbareditor_SRCS
	libkvitoolbareditor.cpp
	CustomizeToolBarsDialog.cpp
)

SET(kvi_module_name kvitoolbareditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
