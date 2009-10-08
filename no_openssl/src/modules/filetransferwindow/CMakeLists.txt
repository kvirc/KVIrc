# CMakeLists for src/modules/filetransferwindow

SUBDIRS(caps)

SET(kvifiletransferwindow_MOC_HDRS
	filetransferwindow.h
)

SET(kvifiletransferwindow_SRCS
	libkvifiletransferwindow.cpp
	filetransferwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvifiletransferwindow_MOC_SRCS ${kvifiletransferwindow_MOC_HDRS})

SET(kvi_module_name kvifiletransferwindow)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
