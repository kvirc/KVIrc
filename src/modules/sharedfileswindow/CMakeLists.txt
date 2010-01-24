# CMakeLists for src/modules/sharedfileswindow

SUBDIRS(caps)

SET(kvisharedfileswindow_MOC_HDRS
	sharedfileswindow.h
)

SET(kvisharedfileswindow_SRCS
	libkvisharedfileswindow.cpp
	sharedfileswindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisharedfileswindow_MOC_SRCS ${kvisharedfileswindow_MOC_HDRS})

SET(kvi_module_name kvisharedfileswindow)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
