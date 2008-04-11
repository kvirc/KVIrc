# CMakeLists for src/modules/http

SET(kvihttp_MOC_HDRS
	httpfiletransfer.h
)

SET(kvihttp_SRCS
	libkvihttp.cpp
	httpfiletransfer.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvihttp_MOC_SRCS ${kvihttp_MOC_HDRS})

SET(kvi_module_name kvihttp)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
