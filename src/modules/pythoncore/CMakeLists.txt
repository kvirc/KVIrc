# CMakeLists for src/modules/pythoncore

#SET(kvipythoncore_MOC_HDRS
#	pythoncoreinterface.h
#)

SET(kvipythoncore_SRCS
	libkvipythoncore.cpp
	kvircmodule.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
#QT4_WRAP_CPP(kvipythoncore_MOC_SRCS ${kvipythoncore_MOC_HDRS})

#this module doesn't need moc by now
SET(KVI_DEFAULT_FRAME_CAPTION "") 

SET(kvi_module_name kvipythoncore)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
