# CMakeLists for src/modules/sharedfilewindow

SET(kvisharedfilewindow_MOC_HDRS
	sharedfileswindow.h
)

SET(kvisharedfilewindow_SRCS
	libkvisharedfileswindow.cpp
	sharedfileswindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisharedfilewindow_MOC_SRCS ${kvisharedfilewindow_MOC_HDRS})

SET(kvi_module_name kvisharedfilewindow)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
