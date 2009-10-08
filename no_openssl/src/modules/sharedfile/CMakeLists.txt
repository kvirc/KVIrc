# CMakeLists for src/modules/sharedfile

SET(kvisharedfile_SRCS
	libkvisharedfile.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisharedfile_MOC_SRCS ${kvisharedfile_MOC_HDRS})

SET(kvi_module_name kvisharedfile)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
