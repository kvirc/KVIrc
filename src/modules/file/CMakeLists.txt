# CMakeLists for src/modules/file

SET(kvifile_SRCS
	libkvifile.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvifile_MOC_SRCS ${kvifile_MOC_HDRS})

SET(kvi_module_name kvifile)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
