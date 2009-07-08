# CMakeLists for src/modules/serverdb

SET(kviserverdb_SRCS
	libkviserverdb.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviserverdb_MOC_SRCS ${kviserverdb_MOC_HDRS})

SET(kvi_module_name kviserverdb)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
