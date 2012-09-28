# CMakeLists for src/modules/pythoncore

SET(kvipythoncore_SRCS
	libkvipythoncore.cpp
	kvircmodule.cpp
)

SET(KVI_DEFAULT_FRAME_CAPTION "") 

SET(kvi_module_name kvipythoncore)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
