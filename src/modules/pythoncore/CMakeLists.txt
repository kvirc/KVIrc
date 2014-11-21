# CMakeLists for src/modules/pythoncore

set(kvipythoncore_SRCS
	libkvipythoncore.cpp
	kvircmodule.cpp
)

set(KVI_DEFAULT_FRAME_CAPTION "")

set(kvi_module_name kvipythoncore)

include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
