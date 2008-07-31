# CMakeLists for src/modules/proxydb

SET(kviproxydb_SRCS
	libkviproxydb.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviproxydb_MOC_SRCS ${kviproxydb_MOC_HDRS})

SET(kvi_module_name kviproxydb)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
