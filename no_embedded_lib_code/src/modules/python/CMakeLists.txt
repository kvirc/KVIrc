# CMakeLists for src/modules/python

SET(kvipython_SRCS
	libkvipython.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvipython_MOC_SRCS ${kvipython_MOC_HDRS})

SET(kvi_module_name kvipython)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
